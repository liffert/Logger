#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QFile>
#include "Models/ListModel.h"
#include "FileSystemWatcher.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>

namespace Models::FileReader {

struct LogLine {
    Q_GADGET
    Q_PROPERTY(int index MEMBER index FINAL CONSTANT)
    Q_PROPERTY(QString text MEMBER text FINAL CONSTANT)

public:
    int index;
    QString text;
};

struct FilteredLogLine : public LogLine {
    Q_GADGET
    Q_PROPERTY(int originalIndex MEMBER originalIndex FINAL CONSTANT)

public:
    int originalIndex;
};

class FileReaderModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Utility::Models::ListModel<LogLine> *model READ model() FINAL CONSTANT)
    Q_PROPERTY(Utility::Models::ListModel<FilteredLogLine> *filteredModel READ filteredModel() FINAL CONSTANT)
    Q_PROPERTY(QString filter MEMBER m_filter NOTIFY filterChanged)

public:
    FileReaderModel(QObject* parent = nullptr);
    ~FileReaderModel();

    Q_INVOKABLE void openFile(const QString& path);

    Utility::Models::ListModel<LogLine>* model();
    Utility::Models::ListModel<FilteredLogLine>* filteredModel();

signals:
    void filterChanged();

private:
    void tryToStartFromTheBeginning();
    void pushToFilteredModel(const LogLine& item);

    QFile m_file;
    QTextStream m_stream;

    Utility::FileSystemWatcher& m_fileWatcher;
    Utility::Models::ListModel<LogLine> m_model;
    Utility::Models::ListModel<FilteredLogLine> m_filteredModel;
    QString m_filter;

    std::mutex m_fileMutex;
    std::unique_ptr<std::jthread> m_thread;
    std::condition_variable m_allowReading;
    std::atomic<bool> m_refilter;

    int m_fileSize = 0;
};

}
