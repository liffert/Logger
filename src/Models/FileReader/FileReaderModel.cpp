#include "FileReaderModel.h"

Models::FileReader::FileReaderModel::FileReaderModel(QObject* parent) :
    QObject(parent),
    m_fileWatcher(Utility::FileSystemWatcher::instance())
{
    qInfo() << __PRETTY_FUNCTION__;
    connect(&m_fileWatcher, &Utility::FileSystemWatcher::fileChanged, this, [this](const auto& path) {
        if (path == m_file.fileName()) {
            if (m_fileMutex.try_lock()) {
                tryToStartFromTheBeginning();
                m_fileMutex.unlock();
            }
            m_allowReading.notify_one();
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
                return !m_stream.atEnd() || stopToken.stop_requested();
            });

            while (!m_stream.atEnd() && !stopToken.stop_requested()) {
                tryToStartFromTheBeginning();

                const auto line = m_stream.readLine();
                if (!line.isEmpty()) {
                    m_model.insert(m_model.rowCount(), line);
                    if (line.contains(m_filter)) {
                        m_filteredModel.insert(m_filteredModel.rowCount(), line);
                    }
                }

                m_fileSize = m_file.size();
            }
        }
    });
}

Utility::Models::ListModel<QString>* Models::FileReader::FileReaderModel::model()
{
    return &m_model;
}

Utility::Models::ListModel<QString>* Models::FileReader::FileReaderModel::filteredModel()
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
