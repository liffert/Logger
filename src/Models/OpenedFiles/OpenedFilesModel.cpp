/*
 * Logger - simple log viewer application.
 * Copyright (C) 2025 Dmytro Martyniuk <digitizenetwork@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "OpenedFilesModel.h"
#include <QFileInfo>
#include <QGuiApplication>

const QString Models::OpenedFiles::OpenedFilesModel::PersistentStorageKeys::FILES_KEY = QStringLiteral("Files");
const QString Models::OpenedFiles::OpenedFilesModel::PersistentStorageKeys::VISIBLE_INDEX_KEY = QStringLiteral("VisibleIndex");

Models::OpenedFiles::OpenedFilesModel::OpenedFilesModel(QObject* parent) :
    QObject(parent),
    m_persistentStorage(QStringLiteral("Logger"), QStringLiteral("FileSystemWatcherSettings")),
    m_fileSystemWatcher(Utility::FileSystemWatcher::instance())
{
    //Register for properly working QSettings serialization
    qRegisterMetaType<QList<FileInfo>>();

    m_model.addUserRole(Qt::UserRole + 1, "name", [](const auto& value) {
        return QVariant::fromValue(value.name);
    });

    const auto files = m_persistentStorage.value(Models::OpenedFiles::OpenedFilesModel::PersistentStorageKeys::FILES_KEY).value<QList<FileInfo>>();
    for (const auto& file : files) {
        addFilePath(file);
    }
    m_currentVisibleIndex = m_persistentStorage.value(Models::OpenedFiles::OpenedFilesModel::PersistentStorageKeys::VISIBLE_INDEX_KEY).toInt();
}

Models::OpenedFiles::OpenedFilesModel::~OpenedFilesModel()
{
    m_persistentStorage.setValue(Models::OpenedFiles::OpenedFilesModel::PersistentStorageKeys::FILES_KEY, QVariant::fromValue(m_model.getRawData()));
    m_persistentStorage.setValue(Models::OpenedFiles::OpenedFilesModel::PersistentStorageKeys::VISIBLE_INDEX_KEY, m_currentVisibleIndex);
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
