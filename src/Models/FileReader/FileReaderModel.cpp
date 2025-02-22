#include "FileReaderModel.h"
#include <QRegularExpression>
#include <QGuiApplication>
#include <QClipboard>

Models::FileReader::FileReaderModel::FileReaderModel(QObject* parent) :
    QObject(parent),
    m_fileWatcher(Utility::FileSystemWatcher::instance())
{
    connect(&m_fileWatcher, &Utility::FileSystemWatcher::fileChanged, this, [this](const auto& path) {
        if (path == m_file.fileName()) {
            if (m_fileMutex.try_lock()) {
                startFromTheBeginningIfNeeded(false, Qt::QueuedConnection);
                m_fileMutex.unlock();
            }
            m_allowReading.notify_one();
        }
    });

    connect(this, &FileReaderModel::filterChanged, this, [this]() {
        if (m_file.isOpen()) {
            //Better mutex or atomic for the usecase?
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
    if (m_file.isOpen()) {
        releaseCurrentFile();
    }

    if (m_filePath.isEmpty()) {
        qInfo() << __PRETTY_FUNCTION__ << " file path is empty";
        return;
    }

    m_file.setFileName(m_filePath);
    m_file.open(QIODevice::ReadOnly);

    m_stream.setDevice(&m_file);
    startFromTheBeginningIfNeeded(true, Qt::DirectConnection);

    m_thread = std::make_unique<std::jthread>(&FileReaderModel::processFile, this);
}

void Models::FileReader::FileReaderModel::processFile(const std::stop_token& stopToken)
{
    while (!stopToken.stop_requested()) {
        QMetaObject::invokeMethod(this, &FileReaderModel::test, Qt::QueuedConnection);
        std::unique_lock lock(m_fileMutex);
        m_allowReading.wait(lock, [this, &stopToken]() {
            return !m_stream.atEnd() || m_refilter || stopToken.stop_requested();
        });

        while (!stopToken.stop_requested()) {
            while (m_refilter && !stopToken.stop_requested()) {
                m_refilter = false;
                QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);

                if (m_filter.isEmpty()) {
                    break;
                }

                //Should not have any performance issues because QList is implicitly shared container meaning.
                //Also provides safety mechanism in case if there for some reason will be a race condition.
                QList<LogLine> data;
                //Block to retreive full data
                QMetaObject::invokeMethod(this, [this, &data]() {
                    return m_model.getRawData();
                }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(data), data));

                for (int i = 0; i < data.size(); i++) {
                    if (m_refilter || stopToken.stop_requested() || startFromTheBeginningIfNeeded(false, Qt::QueuedConnection)) {
                        break;
                    }
                    //UI THREAD FREEZES
                    QMetaObject::invokeMethod(this, &FileReaderModel::pushToFilteredModel, Qt::QueuedConnection, data.at(i), i);
                }
            }

            if (m_stream.atEnd()) {
                break;
            }

            startFromTheBeginningIfNeeded(false, Qt::QueuedConnection);
            QMetaObject::invokeMethod(this, &FileReaderModel::pushToModel, Qt::QueuedConnection, m_stream.readLine());
            m_fileSize = m_file.size();
        }
    }
}

void Models::FileReader::FileReaderModel::updateItemSelection(int index, bool exclusive, bool value)
{
    m_model.updateSelection(index, exclusive, value);
}

void Models::FileReader::FileReaderModel::updateFilteredItemSelection(int index, bool exclusive, bool value)
{
    m_filteredModel.updateSelection(index, exclusive, value);
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

bool Models::FileReader::FileReaderModel::startFromTheBeginningIfNeeded(bool force, Qt::ConnectionType invocationType)
{
    if (force || m_file.size() < m_fileSize) {
        QMetaObject::invokeMethod(this, &FileReaderModel::resetModel, invocationType);
        QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, invocationType);
        m_stream.seek(0);
        m_refilter = false;
        m_fileSize = m_file.size();
        return true;
    }
    return false;
}

void Models::FileReader::FileReaderModel::pushToModel(const QString& text)
{
    if (!text.isEmpty()) {
        const auto modelIndex = m_model.rowCount();
        Models::FileReader::LogLine modelItem({.text = text});
        m_model.insert(modelIndex, modelItem);
        if (!m_filter.isEmpty()) {
            pushToFilteredModel(modelItem, modelIndex);
        }
    }
}

void Models::FileReader::FileReaderModel::pushToFilteredModel(const LogLine& item, int originalIndex)
{
    QRegularExpression regExp(m_filter, QRegularExpression::CaseInsensitiveOption);
    const auto text = item.text;
    if (text.contains(regExp)) {
        m_filteredModel.pushBack({text, false, originalIndex});
    }
}

void Models::FileReader::FileReaderModel::releaseCurrentFile()
{
    if (m_thread) {
        m_thread->request_stop();
    }
    m_allowReading.notify_one();
    m_file.close();
}

void Models::FileReader::FileReaderModel::resetModel()
{
    m_model.reset();
}

void Models::FileReader::FileReaderModel::resetFilteredModel()
{
    m_filteredModel.reset();
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
