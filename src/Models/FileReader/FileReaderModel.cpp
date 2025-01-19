#include "FileReaderModel.h"
#include <QRegularExpression>

Models::FileReader::FileReaderModel::FileReaderModel(QObject* parent) :
    QObject(parent),
    m_fileWatcher(Utility::FileSystemWatcher::instance()),
    m_fontMetrics({})
{
    connect(&m_fileWatcher, &Utility::FileSystemWatcher::fileChanged, this, [this](const auto& path) {
        if (path == m_file.fileName()) {
            if (m_fileMutex.try_lock()) {
                tryToStartFromTheBeginning();
                m_fileMutex.unlock();
            }
            m_allowReading.notify_one();
        }
    });

    connect(this, &FileReaderModel::filterChanged, this, [this]() {
        qInfo() << "MYLOG filter changed " << m_filter;
        if (m_file.isOpen()) {
            m_refilter = true;
        }
    });
}

Models::FileReader::FileReaderModel::~FileReaderModel()
{
    if (m_thread) {
        m_thread->request_stop();
    }
    m_allowReading.notify_one();
    m_fileWatcher.removePath(m_file.fileName());
}

void Models::FileReader::FileReaderModel::openFile(const QString &path)
{
    qInfo() << __PRETTY_FUNCTION__ << " " << path;
    if (m_file.isOpen()) {
        releaseCurrentFile();
    }

    const auto localPath = QUrl(path).toLocalFile();
    m_file.setFileName(localPath);
    m_file.open(QIODevice::ReadOnly);
    if (m_file.isOpen()) {
        m_fileWatcher.addPath(localPath);
    }

    m_stream.setDevice(&m_file);
    tryToStartFromTheBeginning(true);

    //Check if maybe should be moved to QThread to make sure that signal emitting is properly done
    m_thread = std::make_unique<std::jthread>([this](const std::stop_token& stopToken) {
        while (true && !stopToken.stop_requested()) {
            std::unique_lock lock(m_fileMutex);
            m_allowReading.wait(lock, [this, &stopToken]() {
                return !m_stream.atEnd() || m_refilter || stopToken.stop_requested();
            });

            while (!stopToken.stop_requested()) {
                while (m_refilter && !stopToken.stop_requested()) {
                    m_refilter = false;
                    //resetFilteredModel();
                    QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);

                    //NOT THREAD SAFE, NEEDS FIXING.
                    const auto& data = m_model.getRawData();
                    for (int i = 0; i < data.size(); i++) {
                        if (m_refilter || stopToken.stop_requested()) {
                            break;
                        }
                        //pushToFilteredModel(data.at(i), i);
                        QMetaObject::invokeMethod(this, &FileReaderModel::pushToFilteredModel, Qt::QueuedConnection, data.at(i), i);
                    }
                }

                if (!m_stream.atEnd()) {
                    tryToStartFromTheBeginning();
                    //pushToModel(m_stream.readLine());
                    QMetaObject::invokeMethod(this, &FileReaderModel::pushToModel, Qt::QueuedConnection, m_stream.readLine());
                    m_fileSize = m_file.size();
                }
            }
        }
    });
}

Utility::Models::ListModel<Models::FileReader::LogLine>* Models::FileReader::FileReaderModel::model()
{
    return &m_model;
}

Utility::Models::ListModel<Models::FileReader::FilteredLogLine>* Models::FileReader::FileReaderModel::filteredModel()
{
    return &m_filteredModel;
}

void Models::FileReader::FileReaderModel::tryToStartFromTheBeginning(bool force)
{
    if (force || m_file.size() < m_fileSize) {
        //resetModel();
        //resetFilteredModel();
        QMetaObject::invokeMethod(this, &FileReaderModel::resetModel, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);
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
        const auto filteredModelIndex = m_filteredModel.rowCount();
        m_filteredModel.insert(filteredModelIndex, {text, originalIndex});
        setFilteredModelWidth(m_fontMetrics.horizontalAdvance(text), true);
    }
}

void Models::FileReader::FileReaderModel::releaseCurrentFile()
{
    if (m_thread) {
        m_thread->request_stop();
    }
    m_allowReading.notify_one();
    m_fileWatcher.removePath(m_file.fileName());
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
