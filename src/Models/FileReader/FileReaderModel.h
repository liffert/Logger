#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QFile>
#include <QFontMetrics>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include "Models/ListModel.h"
#include "FileSystemWatcher.h"

namespace Models::FileReader {

struct LogLine {
    Q_GADGET
    Q_PROPERTY(QString text MEMBER text FINAL CONSTANT)

public:
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
    Q_PROPERTY(int modelWidth MEMBER m_modelWidth NOTIFY modelWidthChanged)
    Q_PROPERTY(int filteredModelWidth MEMBER m_filteredModelWidth NOTIFY filteredModelWidthChanged)

public:
    FileReaderModel(QObject* parent = nullptr);
    ~FileReaderModel();

    Q_INVOKABLE void openFile(const QString& path);

    Utility::Models::ListModel<LogLine>* model();
    Utility::Models::ListModel<FilteredLogLine>* filteredModel();

signals:
    void filterChanged();
    void modelWidthChanged();
    void filteredModelWidthChanged();

private:
    void tryToStartFromTheBeginning(bool force = false);
    void pushToModel(const QString& text);
    void pushToFilteredModel(const LogLine& item, int originalIndex);
    void releaseCurrentFile();
    void setModelWidth(int value, bool onlyIfHigher = false);
    void setFilteredModelWidth(int value, bool onlyIfHigher = false);
    Q_INVOKABLE void resetModel();
    Q_INVOKABLE void resetFilteredModel();

    QFile m_file;
    QTextStream m_stream;
    QFontMetrics m_fontMetrics;

    Utility::FileSystemWatcher& m_fileWatcher;
    Utility::Models::ListModel<LogLine> m_model;
    Utility::Models::ListModel<FilteredLogLine> m_filteredModel;
    QString m_filter;
    int m_modelWidth = 0;
    int m_filteredModelWidth = 0;

    std::mutex m_fileMutex;
    std::unique_ptr<std::jthread> m_thread;
    std::condition_variable m_allowReading;
    std::atomic<bool> m_refilter;

    int m_fileSize = 0;
};

}
