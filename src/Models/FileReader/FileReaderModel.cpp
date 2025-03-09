#include "FileReaderModel.h"
#include <QRegularExpression>
#include <QGuiApplication>
#include <QClipboard>
#include <chrono>


Models::FileReader::FileReaderModel::FileReaderModel(QObject* parent) :
    QObject(parent),
    m_fileWatcher(Utility::FileSystemWatcher::instance())
{
    connect(&m_fileWatcher, &Utility::FileSystemWatcher::fileChanged, this, [this](const auto& path) {
        if (path == m_filePath) {
            if (m_fileMutex.try_lock()) {
                m_fileMutex.unlock();
                m_forceOneThreadLoop = true;
                m_allowReading.notify_one();
            }
        }
    });

    connect(&Settings::SettingsModel::instance(), &Settings::SettingsModel::coloringPatternsChanged, this, &FileReaderModel::triggerRecoloring);
    connect(this, &FileReaderModel::filterChanged, this, &FileReaderModel::triggerRefiltering);
    connect(this, &FileReaderModel::filePathChanged, this, &FileReaderModel::openFile);
}

Models::FileReader::FileReaderModel::~FileReaderModel()
{
    releaseCurrentFile();
}

void Models::FileReader::FileReaderModel::openFile()
{
    qInfo() << __PRETTY_FUNCTION__ << " " << m_filePath;
    if (!m_threadFinished.load(std::memory_order::relaxed)) {
        releaseCurrentFile();
    }

    if (m_filePath.isEmpty()) {
        qInfo() << __PRETTY_FUNCTION__ << " file path is empty";
        return;
    }

    m_threadFinished.store(false, std::memory_order::relaxed);
    m_thread = std::jthread([this, filePath = m_filePath](const std::stop_token& stopToken) {
        processFile(stopToken, filePath);
        m_threadFinished.store(true, std::memory_order::relaxed);
    });
}

void Models::FileReader::FileReaderModel::processFile(const std::stop_token& stopToken, const QString& filePath)
{
    using namespace std::chrono_literals;

    //TODO_LOW: option to set these values on UI?
    constexpr auto updateRate = 50ms;
    constexpr auto threadSleepThreashold = 2s;

    QList<Settings::ColoringPattern> coloringPatterns;
    QMetaObject::invokeMethod(this, [this]() {
        return Settings::SettingsModel::instance().coloringPatterns();
    }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(coloringPatterns), coloringPatterns));


    QFile file;
    file.setFileName(filePath);
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);

    QString currentFilter = filter();

    startFromTheBeginningIfNeeded(true, stream, file);

    std::unique_lock lock(m_fileMutex);
    while (true) {
        m_allowReading.wait(lock, [this, &stopToken, &file, &stream]() {
            return (file.isOpen() && !stream.atEnd()) || m_refilter.load(std::memory_order::relaxed) || m_recolor.load(std::memory_order::relaxed) || m_forceOneThreadLoop || stopToken.stop_requested();
        });

        if (m_forceOneThreadLoop) {
            m_forceOneThreadLoop = false;
        }

        qInfo() << "thread wake up: " << filePath;
        QList<LogLine> items;
        QList<FilteredLogLine> filteredItems;
        auto startReadingPoint = std::chrono::steady_clock::now();

        while (true) {
            if (stopToken.stop_requested()) {
                return;
            }

            while (m_recolor.load(std::memory_order::relaxed)) {
                if (stopToken.stop_requested()) {
                    return;
                }
                m_recolor.store(false, std::memory_order::relaxed);

                if(startFromTheBeginningIfNeeded(true, stream, file)) {
                    items.clear();
                    filteredItems.clear();
                }

                QMetaObject::invokeMethod(this, [this, &coloringPatterns]() {
                    return Settings::SettingsModel::instance().coloringPatterns();
                }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(coloringPatterns), coloringPatterns));
            }

            while (m_refilter.load(std::memory_order::relaxed)) {
                if (stopToken.stop_requested()) {
                    return;
                }

                m_refilter.store(false, std::memory_order::relaxed);
                filteredItems.clear();
                currentFilter = filter();
                QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);

                if (currentFilter.isEmpty()) {
                    break;
                }

                //Block to retreive full data
                //No performance issues with using copying because QList is implicitly shared container. What is
                //also provides safety mechanism in case if there for some reason will be a race condition and
                //original data gets modified.
                QList<LogLine> data;
                QMetaObject::invokeMethod(this, [this, &data]() {
                    return m_model.getRawData();
                }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(data), data));

                for (int i = 0; i < data.size(); i++) {
                    if (stopToken.stop_requested()) {
                        return;
                    }

                    if (m_refilter.load(std::memory_order::relaxed)) {
                        break;
                    }

                    if (startFromTheBeginningIfNeeded(false, stream, file)) {
                        items.clear();
                        filteredItems.clear();
                        break;
                    }

                    const auto& item = data.at(i);
                    const auto& text = item.text;
                    if (isTextContainsFilter(text, currentFilter)) {
                        filteredItems.push_back({text, false, item.color, i});
                    }

                    const auto currentReadingPoint = std::chrono::steady_clock::now();
                    if ((currentReadingPoint - startReadingPoint > updateRate) && !filteredItems.isEmpty()) {
                        QMetaObject::invokeMethod(this, [this, filteredItems]() {
                            m_filteredModel.pushBack(filteredItems);
                            emit itemsAdded();
                        }, Qt::QueuedConnection);
                        startReadingPoint = currentReadingPoint;
                        filteredItems.clear();
                    }
                }
            }

            if (startFromTheBeginningIfNeeded(false, stream, file)) {
                items.clear();
                filteredItems.clear();
                //To make sure that when we are starting from the beginning new filter and coloring patterns will be used without overhead
                //if such request were made.
                continue;
            };

            if (!stream.atEnd()) {
                const auto text = stream.readLine();
                if (!text.isEmpty()) {
                    const auto color = getColor(text, coloringPatterns);
                    items.push_back({.text = text, .color = color});
                    if (isTextContainsFilter(text, currentFilter)) {
                        filteredItems.push_back({text, false, color, m_currentModelSize});
                    }
                    m_currentModelSize++;
                }
            }

            //Update only with some rate. Sleep the thread only after threashold passed, because
            //there are circumstances under which fileChanged event will be fired with 1s interval
            //making thread refresh rate 1s as a result.
            const auto currentReadingPoint = std::chrono::steady_clock::now();
            const auto timePassed = currentReadingPoint - startReadingPoint;
            const auto triggerItemsAdded = !items.empty() || !filteredItems.empty();
            if (timePassed >= updateRate && triggerItemsAdded) {
                QMetaObject::invokeMethod(this, [this, items, filteredItems]() {
                    m_model.pushBack(items);
                    m_filteredModel.pushBack(filteredItems);
                    emit itemsAdded();
                }, Qt::QueuedConnection);
                items.clear();
                filteredItems.clear();
                startReadingPoint = currentReadingPoint;
            } else if (timePassed >= threadSleepThreashold && stream.atEnd()) {
                qInfo() << "Thread sleep waiting for file update " << filePath;
                break;
            }
        }
    }
}

void Models::FileReader::FileReaderModel::updateItemSelection(int index, bool exclusive, bool value)
{
    m_model.updateSelection(index, exclusive, value);
}

void Models::FileReader::FileReaderModel::updateItemsSelection(int startIndex, int endIndex, bool exclusive, bool value)
{
    m_model.updateSelection(startIndex, endIndex, exclusive, value);
}

void Models::FileReader::FileReaderModel::updateFilteredItemSelection(int index, bool exclusive, bool value)
{
    m_filteredModel.updateSelection(index, exclusive, value);
}

void Models::FileReader::FileReaderModel::updateFilteredItemsSelection(int startIndex, int endIndex, bool exclusive, bool value)
{
    m_filteredModel.updateSelection(startIndex, endIndex, exclusive, value);
}

void Models::FileReader::FileReaderModel::selectAllItems()
{
    selectAll(m_model);
}

void Models::FileReader::FileReaderModel::selectAllFilteredItems()
{
    selectAll(m_filteredModel);
}

void Models::FileReader::FileReaderModel::deselectItems()
{
    deselect(m_model);
}

void Models::FileReader::FileReaderModel::deselectFilteredItems()
{
    deselect(m_filteredModel);
}

void Models::FileReader::FileReaderModel::copyToClipboardSelectedItems()
{
    copyToClipBoard(m_model);
}

void Models::FileReader::FileReaderModel::copyToClipboardSelectedFilteredItems()
{
    copyToClipBoard(m_filteredModel);
}

void Models::FileReader::FileReaderModel::copyAllItems()
{
    auto modelSelection = m_model.getSelection();
    const auto& filteredModelSelection = m_filteredModel.getSelection();
    const auto& filteredModelRawData = m_filteredModel.getRawData();

    for (const auto& selection : filteredModelSelection) {
        if (selection < 0 || selection >= filteredModelRawData.count()) {
            continue;
        }
        modelSelection.insert(filteredModelRawData.at(selection).originalIndex);
    }

    copyToClipBoard(modelSelection, m_model.getRawData());
}

void Models::FileReader::FileReaderModel::setFilter(const QString& filter)
{
    std::lock_guard lock(m_filterMutex);
    if (m_filter != filter) {
        m_filter = filter;
        emit filterChanged();
    }
}

const QString& Models::FileReader::FileReaderModel::filter() const
{
    std::shared_lock readLock(m_filterMutex);
    return m_filter;
}

Utility::Models::SelectionListModel<Models::FileReader::LogLine>* Models::FileReader::FileReaderModel::model()
{
    return &m_model;
}

Utility::Models::SelectionListModel<Models::FileReader::FilteredLogLine>* Models::FileReader::FileReaderModel::filteredModel()
{
    return &m_filteredModel;
}

bool Models::FileReader::FileReaderModel::startFromTheBeginningIfNeeded(bool force, QTextStream& stream, const QFile& file)
{
    const auto currentSize = file.size();
    const auto result = force || (currentSize < m_fileSize);
    if (result) {
        qInfo() << __PRETTY_FUNCTION__ << currentSize << " " << m_fileSize;
        QMetaObject::invokeMethod(this, &FileReaderModel::resetModel, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);
        stream.seek(0);
        m_currentModelSize = 0;
    }
    m_fileSize = currentSize;
    return result;
}

void Models::FileReader::FileReaderModel::triggerRefiltering()
{
    if (!m_refilter) {
        m_refilter = true;
        m_allowReading.notify_one();
    }
}

void Models::FileReader::FileReaderModel::triggerRecoloring()
{
    if (!m_recolor) {
        m_recolor = true;
        m_allowReading.notify_one();
    }
}

QColor Models::FileReader::FileReaderModel::getColor(const QString& text, const QList<Settings::ColoringPattern>& patterns) const
{
    for (const auto& pattern : patterns) {
        if (text.contains(QRegularExpression(pattern.pattern, pattern.caseSensitive ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption))) {
            return pattern.color;
        }
    }
    return {Qt::black};
}

void Models::FileReader::FileReaderModel::releaseCurrentFile()
{
    m_thread.request_stop();
    m_allowReading.notify_one();
    //In case if thread stuck in blocking connection, process all events before this gets destroyed to exit from the thread properly.
    //Consired to use AllEvents to make sure that UI is not frozen if it takes too long to die. But as far as it should not, keep it like this for now.
    while (!m_threadFinished.load(std::memory_order::relaxed)) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    }
}

void Models::FileReader::FileReaderModel::resetModel()
{
    m_model.reset();
    emit modelReset();
}

void Models::FileReader::FileReaderModel::resetFilteredModel()
{
    m_filteredModel.reset();
    emit filteredModelReset();
}

bool Models::FileReader::FileReaderModel::isTextContainsFilter(const QString &text, const QString& filter)
{
    return !filter.isEmpty() && text.contains(QRegularExpression(m_filter, QRegularExpression::CaseInsensitiveOption));
}

template<typename DataType>
void Models::FileReader::FileReaderModel::copyToClipBoard(const Utility::Models::SelectionListModel<DataType>& model) const
{
    copyToClipBoard(model.getSelection(), model.getRawData());
}

template<typename DataType>
void Models::FileReader::FileReaderModel::copyToClipBoard(const std::set<int>& selection, const QList<DataType>& data) const
{
    if (selection.empty()) {
        return;
    }

    QStringList selectedItems;
    for (const auto& selection : selection) {
        if (selection < 0 || selection >= data.count()) {
            continue;
        }
        selectedItems << data.at(selection).text;
    }

    auto* clipboard = QGuiApplication::clipboard();
    clipboard->setText(selectedItems.join(QStringLiteral("\n")));
}

template<typename DataType>
void Models::FileReader::FileReaderModel::deselect(Utility::Models::SelectionListModel<DataType>& model)
{
    const auto selection = model.getSelection();

    for (const auto& selection : selection) {
        model.updateSelection(selection, false, false);
    }
}

template<typename DataType>
void Models::FileReader::FileReaderModel::selectAll(Utility::Models::SelectionListModel<DataType>& model)
{
    for (int i = 0; i < model.rowCount(); i++) {
        model.updateSelection(i, false, true);
    }
}
