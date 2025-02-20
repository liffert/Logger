#include "FileReaderModel.h"
#include <QRegularExpression>
#include <QGuiApplication>
#include <QClipboard>

Models::FileReader::FileReaderModel::FileReaderModel(QObject* parent) :
    QObject(parent),
    m_fileWatcher(Utility::FileSystemWatcher::instance()),
    m_fontMetrics({})
{
    connect(&m_fileWatcher, &Utility::FileSystemWatcher::fileChanged, this, [this](const auto& path) {
        if (path == m_file.fileName()) {
            if (m_fileMutex.try_lock()) {
                startFromTheBeginningIfNeeded(false, Qt::DirectConnection);
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

                QList<LogLine> data;
                //Block to retreive full data
                QMetaObject::invokeMethod(this, [this]() {
                    return m_model.getRawData();
                }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(data), data));

                for (int i = 0; i < data.size(); i++) {
                    if (m_refilter || stopToken.stop_requested()) {
                        break;
                    }
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

void Models::FileReader::FileReaderModel::copyToClipboardSelectedItems()
{
    copyToClipBoard(m_model);
}

void Models::FileReader::FileReaderModel::copyToClipboardSelectedFilteredItems()
{
    copyToClipBoard(m_filteredModel);
}

Utility::Models::ListModel<Models::FileReader::LogLine>* Models::FileReader::FileReaderModel::model()
{
    return &m_model;
}

Utility::Models::ListModel<Models::FileReader::FilteredLogLine>* Models::FileReader::FileReaderModel::filteredModel()
{
    return &m_filteredModel;
}

void Models::FileReader::FileReaderModel::startFromTheBeginningIfNeeded(bool force, Qt::ConnectionType invocationType)
{
    if (force || m_file.size() < m_fileSize) {
        QMetaObject::invokeMethod(this, &FileReaderModel::resetModel, invocationType);
        QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, invocationType);
        m_stream.seek(0);
        m_fileSize = m_file.size();
    }
}

void Models::FileReader::FileReaderModel::pushToModel(const QString& text)
{
    if (!text.isEmpty()) {
        const auto modelIndex = m_model.rowCount();
        Models::FileReader::LogLine modelItem({.text = text});
        m_model.insert(modelIndex, modelItem);
        setModelWidth(m_fontMetrics.horizontalAdvance(text), true);
        pushToFilteredModel(modelItem, modelIndex);
    }
}

void Models::FileReader::FileReaderModel::pushToFilteredModel(const LogLine& item, int originalIndex)
{
    QRegularExpression regExp(m_filter, QRegularExpression::CaseInsensitiveOption);
    const auto text = item.text;
    if (text.contains(regExp)) {
        m_filteredModel.pushBack({text, false, originalIndex});
        setFilteredModelWidth(m_fontMetrics.horizontalAdvance(text), true);
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

void Models::FileReader::FileReaderModel::setModelWidth(int value, bool onlyIfHigher)
{
    if (m_modelWidth != value && (!onlyIfHigher || m_modelWidth < value)) {
        m_modelWidth = value;
        emit modelWidthChanged();
    }
}

void Models::FileReader::FileReaderModel::setFilteredModelWidth(int value, bool onlyIfHigher)
{
    if (m_filteredModelWidth != value && (!onlyIfHigher || m_filteredModelWidth < value)) {
        m_filteredModelWidth = value;
        emit filteredModelWidthChanged();
    }
}

void Models::FileReader::FileReaderModel::resetModel()
{
    m_model.reset();;
    setModelWidth(0);
}

void Models::FileReader::FileReaderModel::resetFilteredModel()
{
    m_filteredModel.reset();
    setFilteredModelWidth(0);
}

template<typename DataType>
void Models::FileReader::FileReaderModel::copyToClipBoard(const Utility::Models::ListModel<DataType>& model) const
{
    qInfo() << __PRETTY_FUNCTION__ << " 1";
    const auto& selection = model.getSelection();
    if (selection.empty()) {
        return;
    }
    qInfo() << __PRETTY_FUNCTION__ << " 2";

    const auto& data = model.getRawData();
    QStringList selectedItems;
    for (const auto& index : selection) {
        selectedItems << data.at(index).text;
    }
    qInfo() << __PRETTY_FUNCTION__ << " 3 " << selectedItems.join("\n");

    auto* clipboard = QGuiApplication::clipboard();
    clipboard->setText(selectedItems.join("\n"));
}
