#pragma once
#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include "Models/ListModel.h"


namespace Utility {

struct OpenedFileInfo {
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name CONSTANT)
    Q_PROPERTY(QString path MEMBER path CONSTANT)
    Q_PROPERTY(QString filter MEMBER filter CONSTANT)

public:
    QString name;
    QString path;
    QString filter;
};

class FileSystemWatcher : public QObject {
    Q_OBJECT
    QML_SINGLETON
    QML_NAMED_ELEMENT(FileSystemWatcher)

    Q_PROPERTY(Utility::Models::ListModel<OpenedFileInfo> *openedFilesModel READ openedFilesModel() CONSTANT)

public:
    static FileSystemWatcher& instance();
    static FileSystemWatcher* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    Q_INVOKABLE void addFilePath(const QString& path, const QString& filter = QLatin1String());
    Q_INVOKABLE void stopWatchingFile(int index);
    Q_INVOKABLE void updateFilter(int index, const QString& filter);
    Utility::Models::ListModel<OpenedFileInfo>* openedFilesModel();

signals:
    void fileChanged(const QString& path);

private:
    FileSystemWatcher(QObject* parent = nullptr);
    ~FileSystemWatcher();

    QString getFileName(const QString& path, int index) const;

    QFileSystemWatcher m_fileSystemWatcher;
    Utility::Models::ListModel<OpenedFileInfo> m_openedFilesModel;
    QSettings m_persistentStorage;
};

}
