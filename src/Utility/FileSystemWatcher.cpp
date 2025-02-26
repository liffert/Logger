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
    const QUrl url(path);
    const auto localPath = url.isLocalFile() ? url.toLocalFile() : path;
    const auto& openedFiles = m_openedFilesModel.getRawData();
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

Utility::FileSystemWatcher::FileSystemWatcher(QObject* parent) :
    QObject(parent),
    m_persistentStorage(QStringLiteral("Logger"), QStringLiteral("FileSystemWatcherSettings"))
{
    m_openedFilesModel.addUserRole(Qt::UserRole + 1, "name", [](const auto& value) {
        return QVariant::fromValue(value.name);
    });

    connect(&m_fileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &FileSystemWatcher::fileChanged);
    const auto openedFiles = m_persistentStorage.value(QStringLiteral("Files")).toStringList();

    for (const auto& filePath : openedFiles) {
        if (QFile::exists(filePath)) {
            addFilePath(filePath);
        }
    }
}

Utility::FileSystemWatcher::~FileSystemWatcher()
{
    const auto& openedFiles = m_openedFilesModel.getRawData();
    QStringList result;
    for (const auto& openedFileInfo : openedFiles) {
        result << openedFileInfo.path;
    }
    m_persistentStorage.setValue(QStringLiteral("Files"), QVariant::fromValue(result));
    m_persistentStorage.sync();
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
