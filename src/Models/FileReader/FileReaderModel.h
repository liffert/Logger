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
    Q_PROPERTY(bool selected MEMBER selected FINAL CONSTANT)

public:
    QString text;
    bool selected = false;
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

    Q_PROPERTY(Utility::Models::ListModel<LogLine> *model READ model() CONSTANT)
    Q_PROPERTY(Utility::Models::ListModel<FilteredLogLine> *filteredModel READ filteredModel() CONSTANT)
    Q_PROPERTY(QString filter MEMBER m_filter NOTIFY filterChanged)
    Q_PROPERTY(QString filePath MEMBER m_filePath NOTIFY filePathChanged)
    Q_PROPERTY(int modelWidth MEMBER m_modelWidth NOTIFY modelWidthChanged)
    Q_PROPERTY(int filteredModelWidth MEMBER m_filteredModelWidth NOTIFY filteredModelWidthChanged)
    Q_PROPERTY(int lineIndexItemWidth MEMBER m_lineIndexItemWidth NOTIFY lineIndexItemWidthChanged)

public:
    FileReaderModel(QObject* parent = nullptr);
    ~FileReaderModel();

    Q_INVOKABLE void updateItemSelection(int index, bool exclusive, bool value);
    Q_INVOKABLE void updateFilteredItemSelection(int index, bool exclusive, bool value);
    Q_INVOKABLE void selectAllItems();
    Q_INVOKABLE void selectAllFilteredItems();
    Q_INVOKABLE void deselectItems();
    Q_INVOKABLE void deselectFilteredItems();
    //Move copy mechanism to separate thread?
    Q_INVOKABLE void copyToClipboardSelectedItems();
    Q_INVOKABLE void copyToClipboardSelectedFilteredItems();
    Q_INVOKABLE void copyAllItems();

    Utility::Models::ListModel<LogLine>* model();
    Utility::Models::ListModel<FilteredLogLine>* filteredModel();

signals:
    void filterChanged();
    void filePathChanged();
    void modelWidthChanged();
    void filteredModelWidthChanged();
    void lineIndexItemWidthChanged();

private:
    void openFile();
    void processFile(const std::stop_token& stopToken);
    void startFromTheBeginningIfNeeded(bool force, Qt::ConnectionType invocationType);
    void pushToModel(const QString& text);
    void pushToFilteredModel(const LogLine& item, int originalIndex);
    void releaseCurrentFile();
    void setModelWidth(int value, bool onlyIfHigher = false);
    void setFilteredModelWidth(int value, bool onlyIfHigher = false);
    void setLineIndexItemWidth(int currentModelCount);
    void resetModel();
    void resetFilteredModel();

    template<typename DataType>
    void copyToClipBoard(const Utility::Models::ListModel<DataType>& model) const;
    template<typename DataType>
    void copyToClipBoard(const std::set<int>& selection, const QList<DataType>& data) const;
    template<typename DataType>
    void deselect(Utility::Models::ListModel<DataType>& model);
    template<typename DataType>
    void selectAll(Utility::Models::ListModel<DataType>& model);

    int getModelWidthFromText(const QString& text) const;

    QFile m_file;
    QTextStream m_stream;
    QFontMetrics m_fontMetrics;

    Utility::FileSystemWatcher& m_fileWatcher;
    Utility::Models::ListModel<LogLine> m_model;
    Utility::Models::ListModel<FilteredLogLine> m_filteredModel;
    QString m_filter;
    QString m_filePath;
    int m_modelWidth = 0;
    int m_filteredModelWidth = 0;
    int m_lineIndexItemWidth = 0;

    std::mutex m_fileMutex;
    std::unique_ptr<std::jthread> m_thread;
    std::condition_variable m_allowReading;
    std::atomic<bool> m_refilter = false;

    int m_fileSize = 0;
};

}
