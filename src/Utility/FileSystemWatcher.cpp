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
    m_openedFilesModel.pushBack(getFileName(path));
    m_fileSystemWatcher.addPath(path);
}

void Utility::FileSystemWatcher::removeFilePath(const QString& path)
{
    m_openedFilesModel.remove(getFileName(path));
    m_fileSystemWatcher.removePath(path);
}

Utility::Models::ListModel<QString>* Utility::FileSystemWatcher::openedFilesModel()
{
    return &m_openedFilesModel;
}

Utility::FileSystemWatcher::FileSystemWatcher(QObject* parent) : QObject(parent) {
    connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &FileSystemWatcher::fileChanged);
}

QString Utility::FileSystemWatcher::getFileName(const QString& path) const
{
    QFileInfo fileInfo(path);
    return fileInfo.fileName();
}
