#pragma once

#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include "Models/ListModel.h"
#include "FileSystemWatcher.h"


namespace Models::OpenedFiles {

//TODO: serialize whole object
struct FileInfo {
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(QString path MEMBER path CONSTANT)
    Q_PROPERTY(QString filter MEMBER filter CONSTANT)

public:
    QString name;
    QString path;
    QString filter;
};

class OpenedFilesModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Utility::Models::ListModel<FileInfo> *model READ model() CONSTANT)

public:
    OpenedFilesModel(QObject* parent = nullptr);
    ~OpenedFilesModel();
    Q_INVOKABLE void addFilePath(const QString& path, const QString& filter = QLatin1String());
    Q_INVOKABLE void stopWatchingFile(int index);
    Q_INVOKABLE void updateFilter(int index, const QString& filter);
    Utility::Models::ListModel<FileInfo>* model();

signals:
    void fileChanged(const QString& path);

private:
    QString getFileName(const QString& path, int index) const;

    Utility::Models::ListModel<FileInfo> m_model;
    Utility::FileSystemWatcher& m_fileSystemWatcher;
    QSettings m_persistentStorage;
};

}
