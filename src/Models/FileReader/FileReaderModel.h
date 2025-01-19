#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QFile>
#include "Models/ListModel.h"
#include "FileSystemWatcher.h"
#include <condition_variable>
#include <mutex>
#include <thread>

namespace Models::FileReader {

class FileReaderModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Utility::Models::ListModel<QString> *model READ model() FINAL CONSTANT)
    Q_PROPERTY(Utility::Models::ListModel<QString> *filteredModel READ filteredModel() FINAL CONSTANT)
    Q_PROPERTY(QString filter MEMBER m_filter NOTIFY filterChanged)

public:
    FileReaderModel(QObject* parent = nullptr);
    ~FileReaderModel();

    Q_INVOKABLE void openFile(const QString& path);

    Utility::Models::ListModel<QString>* model();
    Utility::Models::ListModel<QString>* filteredModel();

signals:
    void filterChanged();

private:
    void tryToStartFromTheBeginning();

    QFile m_file;
    QTextStream m_stream;

    Utility::FileSystemWatcher& m_fileWatcher;
    Utility::Models::ListModel<QString> m_model;
    Utility::Models::ListModel<QString> m_filteredModel;
    QString m_filter;

    std::mutex m_fileMutex;
    std::unique_ptr<std::jthread> m_thread;
    std::condition_variable m_allowReading;

    int m_fileSize = 0;
};

}
