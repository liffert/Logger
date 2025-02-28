#include "OpenedFilesModel.h"
#include <QFileInfo>
#include <QGuiApplication>

Models::OpenedFiles::OpenedFilesModel::OpenedFilesModel(QObject* parent) :
    QObject(parent),
    m_persistentStorage(QStringLiteral("Logger"), QStringLiteral("FileSystemWatcherSettings")),
    m_fileSystemWatcher(Utility::FileSystemWatcher::instance())
{
    m_model.addUserRole(Qt::UserRole + 1, "name", [](const auto& value) {
        return QVariant::fromValue(value.name);
    });

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

Models::OpenedFiles::OpenedFilesModel::~OpenedFilesModel()
{
    const auto& openedFiles = m_model.getRawData();

    m_persistentStorage.clear();
    m_persistentStorage.beginWriteArray("Files");
    for (int i = 0; i < openedFiles.count(); i++) {
        m_persistentStorage.setArrayIndex(i);
        m_persistentStorage.setValue(QStringLiteral("Path"), openedFiles.at(i).path);
        m_persistentStorage.setValue(QStringLiteral("Filter"), openedFiles.at(i).filter);
    }
    m_persistentStorage.endArray();
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
    const auto& openedFiles = m_model.getRawData();
    if (index < 0 || index >= openedFiles.count()) {
        qWarning() << __PRETTY_FUNCTION__ << " index is out of bounds: " << index;
        return;
    }

    auto data = openedFiles.at(index);
    data.filter = filter;
    m_model.update(index, data);
}

Utility::Models::ListModel<Models::OpenedFiles::FileInfo>* Models::OpenedFiles::OpenedFilesModel::model()
{
    return &m_model;
}

QString Models::OpenedFiles::OpenedFilesModel::getFileName(const QString& path, int index) const
{
    QFileInfo fileInfo(path);
    auto result = fileInfo.fileName();
    if (index > 0) {
        result.append(QStringLiteral("(%1)").arg(index));
    }
    return result;
}
