#pragma once
#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include "Models/ListModel.h"


namespace Utility {

struct OpenedFileInfo {
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name FINAL CONSTANT)
    Q_PROPERTY(QString path MEMBER path FINAL CONSTANT)

public:
    QString name;
    QString path;
};

class FileSystemWatcher : public QObject {
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(FileSystemWatcher)

    Q_PROPERTY(Utility::Models::ListModel<OpenedFileInfo> *openedFilesModel READ openedFilesModel() CONSTANT)

public:
    static FileSystemWatcher& instance();
    static FileSystemWatcher* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    Q_INVOKABLE void addFilePath(const QString& path);
    Q_INVOKABLE void stopWatchingFile(int index);
    Utility::Models::ListModel<OpenedFileInfo>* openedFilesModel();

signals:
    void fileChanged(const QString& path);

private:
    FileSystemWatcher(QObject* parent = nullptr);

    QString getFileName(const QString& path, int index) const;

    QFileSystemWatcher m_fileSystemWatcher;
    Utility::Models::ListModel<OpenedFileInfo> m_openedFilesModel;
};

}
