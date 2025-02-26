#include "FileSystemWatcher.h"
#include <QFileInfo>

//TODO: check if it really should be present as Utility class and not as model class
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

void Utility::FileSystemWatcher::addFilePath(const QString& path, const QString& filter)
{
    const QUrl url(path);
    const auto localPath = url.isLocalFile() ? url.toLocalFile() : path;
    const auto& openedFiles = m_openedFilesModel.getRawData();
    const auto amountOfSameFiles = std::count_if(openedFiles.cbegin(), openedFiles.cend(), [&localPath](const auto& value) {
        return value.path == localPath;
    });
    m_openedFilesModel.pushBack({getFileName(localPath, amountOfSameFiles), localPath, filter});
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

void Utility::FileSystemWatcher::updateFilter(int index, const QString& filter)
{
    const auto& openedFiles = m_openedFilesModel.getRawData();
    if (index < 0 || index >= openedFiles.count()) {
        qWarning() << __PRETTY_FUNCTION__ << " index is out of bounds: " << index;
        return;
    }

    auto data = openedFiles.at(index);
    data.filter = filter;
    m_openedFilesModel.update(index, data);
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

    const auto filesSize = m_persistentStorage.beginReadArray(QStringLiteral("Files"));
    for (int i = 0; i < filesSize; i++) {
        m_persistentStorage.setArrayIndex(i);
        auto filePath = m_persistentStorage.value(QStringLiteral("Path")).toString();
        if (QFile::exists(filePath)) {
            addFilePath(filePath, m_persistentStorage.value(QStringLiteral("Filter")).toString());
        }
    }
    m_persistentStorage.endArray();
}

Utility::FileSystemWatcher::~FileSystemWatcher()
{
    const auto& openedFiles = m_openedFilesModel.getRawData();

    m_persistentStorage.clear();
    m_persistentStorage.beginWriteArray("Files");
    for (int i = 0; i < openedFiles.count(); i++) {
        m_persistentStorage.setArrayIndex(i);
        m_persistentStorage.setValue(QStringLiteral("Path"), openedFiles.at(i).path);
        m_persistentStorage.setValue(QStringLiteral("Filter"), openedFiles.at(i).filter);
    }
    m_persistentStorage.endArray();
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
