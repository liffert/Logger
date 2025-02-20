#include "FileSystemWatcher.h"
#include <QFileInfo>

Utility::FileSystemWatcher& Utility::FileSystemWatcher::instance()
{
    static FileSystemWatcher instance;
    return instance;
}

Utility::FileSystemWatcher *Utility::FileSystemWatcher::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);

    auto& objectInstance = instance();
    QJSEngine::setObjectOwnership(&objectInstance, QJSEngine::CppOwnership);
    return &instance();
}

void Utility::FileSystemWatcher::addFilePath(const QString& path)
{
    const auto& openedFiles = m_openedFilesModel.getRawData();

    const auto localPath = QUrl(path).toLocalFile();
    const auto amountOfSameFiles = std::count_if(openedFiles.cbegin(), openedFiles.cend(), [&localPath](const auto& value) {
        return value.path == localPath;
    });
    m_openedFilesModel.pushBack({getFileName(localPath, amountOfSameFiles), localPath});
    m_fileSystemWatcher.addPath(localPath);
}

void Utility::FileSystemWatcher::stopWatchingFile(int index)
{
    const auto& openedFiles = m_openedFilesModel.getRawData();
    if (index < 0 || index > openedFiles.count()) {
        qWarning() << __PRETTY_FUNCTION__ << " index is out of bounds " << index;
        return;
    }

    const auto filePath = openedFiles.at(index).path;
    m_openedFilesModel.remove(index);

    if (std::none_of(openedFiles.cbegin(), openedFiles.cend(), [&filePath](const auto& value) { return value.path == filePath; })) {
        m_fileSystemWatcher.removePath(filePath);
    }
}

Utility::Models::ListModel<Utility::OpenedFileInfo>* Utility::FileSystemWatcher::openedFilesModel()
{
    return &m_openedFilesModel;
}

Utility::FileSystemWatcher::FileSystemWatcher(QObject* parent) : QObject(parent) {
    connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &FileSystemWatcher::fileChanged);
}

QString Utility::FileSystemWatcher::getFileName(const QString& path, int index) const
{
    QFileInfo fileInfo(path);
    auto result = fileInfo.fileName();
    if (index > 0) {
        result.append(QStringLiteral("(%1)").arg(index));
    }
    return result;
}
