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
        if (path == m_file.fileName()) {
            if (m_fileMutex.try_lock()) {
                startFromTheBeginningIfNeeded(false);
                m_fileMutex.unlock();
            }
            m_allowReading.notify_one();
        }
    });

    connect(this, &FileReaderModel::filterChanged, this, [this]() {
        if (m_file.isOpen()) {
            if (!m_refilter) {
                m_refilter = true;
                m_allowReading.notify_one();
            }
        }
    });

    connect(this, &FileReaderModel::filePathChanged, this, &FileReaderModel::openFile);
}

Models::FileReader::FileReaderModel::~FileReaderModel()
{
    releaseCurrentFile();
}

void Models::FileReader::FileReaderModel::openFile()
{
    qInfo() << __PRETTY_FUNCTION__ << " " << m_filePath;
    if (!m_threadFinished) {
        releaseCurrentFile();
    }

    if (m_filePath.isEmpty()) {
        qInfo() << __PRETTY_FUNCTION__ << " file path is empty";
        return;
    }

    m_file.setFileName(m_filePath);
    m_file.open(QIODevice::ReadOnly);

    m_stream.setDevice(&m_file);
    startFromTheBeginningIfNeeded(true);

    m_threadFinished = false;
    m_thread = std::jthread([this](const std::stop_token& stopToken) {
        processFile(stopToken);
        //Atomic flag looks quite ugly. Maybe to use std::packaged_task with std::future,
        //but in general from performance perspective it will not be any better...
        m_threadFinished = true;
    });
}

void Models::FileReader::FileReaderModel::processFile(const std::stop_token& stopToken)
{
    using namespace std::chrono_literals;

    m_currentModelSize = 0;
    while (true) {
        std::unique_lock lock(m_fileMutex);
        m_allowReading.wait(lock, [this, &stopToken]() {
            return (m_file.isOpen() && !m_stream.atEnd()) || m_refilter || stopToken.stop_requested();
        });

        QList<LogLine> items;
        QList<FilteredLogLine> filteredItems;
        const auto startReadingPoint = std::chrono::steady_clock::now();

        while (true) {
            if (stopToken.stop_requested()) {
                return;
            }

            while (m_refilter) {
                if (stopToken.stop_requested()) {
                    return;
                }

                m_refilter = false;
                filteredItems.clear();
                QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);

                if (m_filter.isEmpty()) {
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

                    if (m_refilter) {
                        break;
                    }

                    if (startFromTheBeginningIfNeeded(false)) {
                        items.clear();
                        filteredItems.clear();
                        break;
                    }

                    const auto& item = data.at(i);
                    const auto& text = item.text;
                    if (isTextContainsFilter(text)) {
                        filteredItems.push_back({text, false, item.color, i});
                    }
                }
            }

            if (m_stream.atEnd()) {
                break;
            }

            if (startFromTheBeginningIfNeeded(false)) {
                items.clear();
                filteredItems.clear();
            };

            const auto text = m_stream.readLine();
            if (!text.isEmpty()) {
                const auto color = getColor(text);
                items.push_back({.text = text, .color = color});
                if (isTextContainsFilter(text)) {
                    filteredItems.push_back({text, false, color, m_currentModelSize});
                }
                m_currentModelSize++;
            }
            m_fileSize = m_file.size();
            if (std::chrono::steady_clock::now() - startReadingPoint > 1000ms) {
                break;
            }
        }

        QMetaObject::invokeMethod(this, [this, items, filteredItems]() {
            m_model.pushBack(items);
            m_filteredModel.pushBack(filteredItems);
        }, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, &FileReaderModel::itemsAdded, Qt::QueuedConnection);
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

Utility::Models::ListModel<Models::FileReader::LogLine>* Models::FileReader::FileReaderModel::model()
{
    return &m_model;
}

Utility::Models::ListModel<Models::FileReader::FilteredLogLine>* Models::FileReader::FileReaderModel::filteredModel()
{
    return &m_filteredModel;
}

bool Models::FileReader::FileReaderModel::startFromTheBeginningIfNeeded(bool force)
{
    if (force || m_file.size() < m_fileSize) {
        QMetaObject::invokeMethod(this, &FileReaderModel::resetModel, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);
        m_stream.seek(0);
        m_refilter = false;
        m_fileSize = m_file.size();
        m_currentModelSize = 0;
        return true;
    }
    return false;
}

QColor Models::FileReader::FileReaderModel::getColor(const QString &text) const
{
    static const QList<QPair<QString, QColor>> patterns = {
                                                           {":RQ :", QColor(Qt::magenta)},
                                                           {":RP :", QColor(Qt::blue)},
                                                           {":EV :", QColor(Qt::darkCyan)},//TODO: Cyan in old view app, but really hard to read, so maybe dark cyan is better
                                                           {"WARN", QColor(QColorConstants::Svg::orange)},
                                                           {"CRIT", QColor(Qt::red)},
                                                           {"FATAL", QColor(Qt::darkRed)},
                                                           {"MYLOG", QColor(Qt::darkGreen)},
                                                           {"if1verbose", QColor(Qt::darkBlue)}
    };

    for (const auto& pattern : patterns) {
        if (text.contains(QRegularExpression(pattern.first, QRegularExpression::CaseInsensitiveOption))) {
            return pattern.second;
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
    while (!m_threadFinished) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    }
    m_file.close();
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

bool Models::FileReader::FileReaderModel::isTextContainsFilter(const QString &text)
{
    return !m_filter.isEmpty() && text.contains(QRegularExpression(m_filter, QRegularExpression::CaseInsensitiveOption));
}

template<typename DataType>
void Models::FileReader::FileReaderModel::copyToClipBoard(const Utility::Models::ListModel<DataType>& model) const
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
    clipboard->setText(selectedItems.join("\n"));
}

template<typename DataType>
void Models::FileReader::FileReaderModel::deselect(Utility::Models::ListModel<DataType>& model)
{
    const auto selection = model.getSelection();

    for (const auto& selection : selection) {
        model.updateSelection(selection, false, false);
    }
}

template<typename DataType>
void Models::FileReader::FileReaderModel::selectAll(Utility::Models::ListModel<DataType>& model)
{
    for (int i = 0; i < model.rowCount(); i++) {
        model.updateSelection(i, false, true);
    }
}
