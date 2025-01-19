#include "FileReaderModel.h"
#include <QSortFilterProxyModel>

Models::FileReader::FileReaderModel::FileReaderModel(QObject* parent) :
    QObject(parent),
    m_fileWatcher(Utility::FileSystemWatcher::instance())
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
        m_refilter = true;
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
        qInfo() << "Return as not supported at the moment";
        return;
    }

    const auto localPath = QUrl(path).toLocalFile();
    m_file.setFileName(localPath);
    m_file.open(QIODevice::ReadOnly);
    if (m_file.isOpen()) {
        m_fileWatcher.addPath(localPath);
    }

    m_stream.setDevice(&m_file);
    m_model.reset();

    m_thread = std::make_unique<std::jthread>([this](const std::stop_token& stopToken) {
        while (true && !stopToken.stop_requested()) {
            std::unique_lock lock(m_fileMutex);
            m_allowReading.wait(lock, [this, &stopToken]() {
                return !m_stream.atEnd() || m_refilter || stopToken.stop_requested();
            });

            while (!stopToken.stop_requested()) {
                while (m_refilter && !stopToken.stop_requested()) {
                    m_refilter = false;
                    m_filteredModel.reset();

                    const auto& data = m_model.getRawData();
                    for (const auto& item : data) {
                        if (m_refilter || stopToken.stop_requested()) {
                            break;
                        }
                        pushToFilteredModel(item);
                    }
                }

                if (!m_stream.atEnd()) {
                    tryToStartFromTheBeginning();
                    const auto line = m_stream.readLine();
                    if (!line.isEmpty()) {
                        const auto modelIndex = m_model.rowCount();
                        Models::FileReader::LogLine modelItem({.index = modelIndex, .text = line});
                        m_model.insert(modelIndex, modelItem);
                        pushToFilteredModel(modelItem);
                    }
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

void Models::FileReader::FileReaderModel::tryToStartFromTheBeginning()
{
    if (m_file.size() < m_fileSize) {
        m_model.reset();
        m_filteredModel.reset();
        m_stream.seek(0);
        m_fileSize = m_file.size();
    }
}

void Models::FileReader::FileReaderModel::pushToFilteredModel(const LogLine& item)
{
    if (item.text.contains(m_filter)) {
        Models::FileReader::FilteredLogLine filteredItem;
        filteredItem.text = item.text;
        filteredItem.originalIndex = item.index;
        const auto filteredModelIndex = m_filteredModel.rowCount();
        filteredItem.index = filteredModelIndex;
        m_filteredModel.insert(filteredModelIndex, filteredItem);
    }
}
