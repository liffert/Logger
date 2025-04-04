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

#pragma once

#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include "Models/ListModel.h"
#include "FileSystemWatcher.h"


namespace Models::OpenedFiles {

//TODO_LOW: store geometry of views
struct FileInfo {
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(QString path MEMBER path CONSTANT)
    Q_PROPERTY(QString filter MEMBER filter CONSTANT)
    Q_PROPERTY(bool fullFileViewAutoScrollEnabled MEMBER fullFileViewAutoScrollEnabled CONSTANT)
    Q_PROPERTY(bool filteredFileViewAutoScrollEnabled MEMBER filteredFileViewAutoScrollEnabled CONSTANT)

public:
    QString name;
    QString path;
    QString filter;
    bool fullFileViewAutoScrollEnabled = true;
    bool filteredFileViewAutoScrollEnabled = true;

    friend QDataStream& operator<<(QDataStream& stream, const FileInfo& object);
    friend QDataStream& operator>>(QDataStream& stream, FileInfo& object);
};

class OpenedFilesModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Utility::Models::ListModel<FileInfo> *model READ model() CONSTANT)
    Q_PROPERTY(int currentVisibleIndex MEMBER m_currentVisibleIndex NOTIFY currentVisibleIndexChanged)

public:
    OpenedFilesModel(QObject* parent = nullptr);
    ~OpenedFilesModel();
    Q_INVOKABLE void addFilePath(const QString& path, const QString& filter = QLatin1String());
    Q_INVOKABLE void addFilePath(const FileInfo& fileInfo);
    Q_INVOKABLE void stopWatchingFile(int index);
    Q_INVOKABLE void updateFilter(int index, const QString& filter);
    Q_INVOKABLE void updateFullFileViewAutoScroll(int index, bool value);
    Q_INVOKABLE void updateFilteredFileViewAutoScroll(int index, bool value);
    Utility::Models::ListModel<FileInfo>* model();

signals:
    void fileChanged(const QString& path);
    void currentVisibleIndexChanged();

private:
    struct PersistentStorageKeys {
        static const QString FILES_KEY;
        static const QString VISIBLE_INDEX_KEY;
    };

    QString getFileName(const QString& path, int index) const;

    Utility::Models::ListModel<FileInfo> m_model;
    Utility::FileSystemWatcher& m_fileSystemWatcher;
    QSettings m_persistentStorage;
    int m_currentVisibleIndex = -1;
};

inline QDataStream& operator<<(QDataStream& stream, const FileInfo& object)
{
    stream << object.name;
    stream << object.path;
    stream << object.filter;
    stream << object.fullFileViewAutoScrollEnabled;
    stream << object.filteredFileViewAutoScrollEnabled;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, FileInfo& object)
{
    stream >> object.name;
    stream >> object.path;
    stream >> object.filter;
    stream >> object.fullFileViewAutoScrollEnabled;
    stream >> object.filteredFileViewAutoScrollEnabled;
    return stream;
}

}
