#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QFile>
#include <QColor>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include "Models/ListModel.h"
#include "FileSystemWatcher.h"

namespace Models::FileReader {

struct LogLine {
    Q_GADGET
    Q_PROPERTY(QString text MEMBER text CONSTANT)
    Q_PROPERTY(bool selected MEMBER selected CONSTANT)
    Q_PROPERTY(QColor color MEMBER color CONSTANT)

public:
    QString text;
    bool selected = false;
    QColor color = {Qt::black};
};

struct FilteredLogLine : public LogLine {
    Q_GADGET
    Q_PROPERTY(int originalIndex MEMBER originalIndex CONSTANT)

public:
    int originalIndex;
};

class FileReaderModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Utility::Models::ListModel<LogLine> *model READ model() CONSTANT)
    Q_PROPERTY(Utility::Models::ListModel<FilteredLogLine> *filteredModel READ filteredModel() CONSTANT)
    Q_PROPERTY(QString filter MEMBER m_filter NOTIFY filterChanged)

public:
    FileReaderModel(QObject* parent = nullptr);
    ~FileReaderModel();

    Q_INVOKABLE void openFile(const QString& filePath);
    Q_INVOKABLE void updateItemSelection(int index, bool exclusive, bool value);
    Q_INVOKABLE void updateFilteredItemSelection(int index, bool exclusive, bool value);
    Q_INVOKABLE void selectAllItems();
    Q_INVOKABLE void selectAllFilteredItems();
    Q_INVOKABLE void deselectItems();
    Q_INVOKABLE void deselectFilteredItems();
    //Move copy mechanism to separate thread? Check performance
    Q_INVOKABLE void copyToClipboardSelectedItems();
    Q_INVOKABLE void copyToClipboardSelectedFilteredItems();
    Q_INVOKABLE void copyAllItems();

    Utility::Models::ListModel<LogLine>* model();
    Utility::Models::ListModel<FilteredLogLine>* filteredModel();

signals:
    void filterChanged();
    void filePathChanged();
    void itemsAdded();

private:
    void processFile(const std::stop_token& stopToken);
    void releaseCurrentFile();
    void resetModel();
    void resetFilteredModel();
    bool isTextContainsFilter(const QString& text);
    bool startFromTheBeginningIfNeeded(bool force);
    QColor getColor(const QString& text) const;

    template<typename DataType>
    void copyToClipBoard(const Utility::Models::ListModel<DataType>& model) const;
    template<typename DataType>
    void copyToClipBoard(const std::set<int>& selection, const QList<DataType>& data) const;
    template<typename DataType>
    void deselect(Utility::Models::ListModel<DataType>& model);
    template<typename DataType>
    void selectAll(Utility::Models::ListModel<DataType>& model);

    QFile m_file;
    QTextStream m_stream;

    Utility::FileSystemWatcher& m_fileWatcher;
    Utility::Models::ListModel<LogLine> m_model;
    Utility::Models::ListModel<FilteredLogLine> m_filteredModel;
    QString m_filter;
    QString m_filePath;

    std::mutex m_fileMutex;
    std::jthread m_thread;
    std::condition_variable m_allowReading;
    std::atomic<bool> m_refilter = false;
    std::atomic<bool> m_threadFinished = false;

    int m_fileSize = 0;
    int m_currentModelSize = 0;
};

}
