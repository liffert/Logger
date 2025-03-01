#include "OpenedFilesModel.h"
#include <QFileInfo>
#include <QGuiApplication>

Models::OpenedFiles::OpenedFilesModel::OpenedFilesModel(QObject* parent) :
    QObject(parent),
    m_persistentStorage(QStringLiteral("Logger"), QStringLiteral("FileSystemWatcherSettings")),
    m_fileSystemWatcher(Utility::FileSystemWatcher::instance())
{
    //Register for working QSettings serialization properly
    qRegisterMetaType<QList<FileInfo>>();

    m_model.addUserRole(Qt::UserRole + 1, "name", [](const auto& value) {
        return QVariant::fromValue(value.name);
    });

    const auto files = m_persistentStorage.value(QStringLiteral("Files")).value<QList<FileInfo>>();
    for (const auto& file : files) {
        addFilePath(file);
    }
    m_currentVisibleIndex = m_persistentStorage.value(QStringLiteral("VisibleIndex")).toInt();
    emit currentVisibleIndexChanged();
}

Models::OpenedFiles::OpenedFilesModel::~OpenedFilesModel()
{
    m_persistentStorage.setValue(QStringLiteral("Files"), QVariant::fromValue(m_model.getRawData()));
    m_persistentStorage.setValue(QStringLiteral("VisibleIndex"), m_currentVisibleIndex);
}

void Models::OpenedFiles::OpenedFilesModel::addFilePath(const QString& path, const QString& filter)
{
    const QUrl url(path);
    const auto localPath = url.isLocalFile() ? url.toLocalFile() : path;
    const auto& openedFiles = m_model.getRawData();
    const auto amountOfSameFiles = std::count_if(openedFiles.cbegin(), openedFiles.cend(), [&localPath](const auto& value) {
        return value.path == localPath;
    });
    m_model.pushBack({getFileName(localPath, amountOfSameFiles), localPath, filter});
    m_fileSystemWatcher.addPath(localPath);
}

void Models::OpenedFiles::OpenedFilesModel::addFilePath(const FileInfo& fileInfo)
{
    const auto& path = fileInfo.path;
    if (QFile::exists(path)) {
        m_model.pushBack(fileInfo);
        m_fileSystemWatcher.addPath(path);
    }
}

void Models::OpenedFiles::OpenedFilesModel::stopWatchingFile(int index)
{
    const auto& openedFiles = m_model.getRawData();
    if (index < 0 || index > openedFiles.count()) {
        qWarning() << __PRETTY_FUNCTION__ << " index is out of bounds " << index;
        return;
    }

    const auto filePath = openedFiles.at(index).path;
    m_model.remove(index);

    if (std::none_of(openedFiles.cbegin(), openedFiles.cend(), [&filePath](const auto& value) { return value.path == filePath; })) {
        m_fileSystemWatcher.removePath(filePath);
    }
}

void Models::OpenedFiles::OpenedFilesModel::updateFilter(int index, const QString& filter)
{
    m_model.update(index, [&filter](auto& data) {
        data.filter = filter;
    });
}

void Models::OpenedFiles::OpenedFilesModel::updateFullFileViewAutoScroll(int index, bool value)
{
    m_model.update(index, [value](auto& data) {
        data.fullFileViewAutoScrollEnabled = value;
    });
}

void Models::OpenedFiles::OpenedFilesModel::updateFilteredFileViewAutoScroll(int index, bool value)
{
    m_model.update(index, [value](auto& data) {
        data.filteredFileViewAutoScrollEnabled = value;
    });
}

Utility::Models::ListModel<Models::OpenedFiles::FileInfo>* Models::OpenedFiles::OpenedFilesModel::model()
{
    return &m_model;
}

QString Models::OpenedFiles::OpenedFilesModel::getFileName(const QString& path, int index) const
{
    QFileInfo fileInfo(path);
    auto result = fileInfo.fileName();
    return result;
}
