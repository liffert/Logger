#pragma once
#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include "Models/ListModel.h"


namespace Utility {

class FileSystemWatcher : public QObject {
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(FileSystemWatcher)

    Q_PROPERTY(Utility::Models::ListModel<QString> *openedFilesModel READ openedFilesModel() CONSTANT)

public:
    static FileSystemWatcher& instance();
    static FileSystemWatcher* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    void addFilePath(const QString& path);
    void removeFilePath(const QString& path);
    Utility::Models::ListModel<QString>* openedFilesModel();

signals:
    void fileChanged(const QString& path);

private:
    FileSystemWatcher(QObject* parent = nullptr);

    QString getFileName(const QString& path) const;

    QFileSystemWatcher m_fileSystemWatcher;
    Utility::Models::ListModel<QString> m_openedFilesModel;
};

}
