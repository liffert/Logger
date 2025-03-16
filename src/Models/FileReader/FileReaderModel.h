#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QFile>
#include <QColor>
#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include "Models/SelectionListModel.h"
#include "FileSystemWatcher.h"
#include "Settings/SettingsModel.h"
#include "Style.h"

namespace Models::FileReader {

struct LogLine {
    Q_GADGET
    Q_PROPERTY(QString text MEMBER text CONSTANT)
    Q_PROPERTY(bool selected MEMBER selected CONSTANT)
    Q_PROPERTY(QColor color MEMBER color CONSTANT)

public:
    QString text;
    bool selected = false;
    QColor color = Utility::Style::instance().regularTextColor();
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

    Q_PROPERTY(Utility::Models::SelectionListModel<LogLine> *model READ model() CONSTANT)
    Q_PROPERTY(Utility::Models::SelectionListModel<FilteredLogLine> *filteredModel READ filteredModel() CONSTANT)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString filePath MEMBER m_filePath NOTIFY filePathChanged)
    Q_PROPERTY(int indexLineWidth MEMBER m_indexLineWidth NOTIFY indexLineWidthChanged)

public:
    FileReaderModel(QObject* parent = nullptr);
    ~FileReaderModel();

    Q_INVOKABLE void updateItemSelection(int index, bool exclusive, bool value);
    Q_INVOKABLE void updateItemsSelection(int startIndex, int endIndex, bool exclusive, bool value);
    Q_INVOKABLE void updateFilteredItemSelection(int index, bool exclusive, bool value);
    Q_INVOKABLE void updateFilteredItemsSelection(int startIndex, int endIndex, bool exclusive, bool value);
    Q_INVOKABLE void selectAllItems();
    Q_INVOKABLE void selectAllFilteredItems();
    Q_INVOKABLE void deselectItems();
    Q_INVOKABLE void deselectFilteredItems();
    //Copy looks okey in gui thread. Test on 400000 lines gives instant copy. No reason to invest in separate thread copying.
    //Return here only in case of real issues.
    Q_INVOKABLE void copyToClipboardSelectedItems();
    Q_INVOKABLE void copyToClipboardSelectedFilteredItems();
    Q_INVOKABLE void copyAllItems();

    Q_INVOKABLE QColor getColor(const QString& filter);

    void setFilter(const QString& filter);
    const QString& filter() const;

    Utility::Models::SelectionListModel<LogLine>* model();
    Utility::Models::SelectionListModel<FilteredLogLine>* filteredModel();

signals:
    void filterChanged();
    void filePathChanged();
    void itemsAdded();
    void modelReset();
    void filteredModelReset();
    void indexLineWidthChanged();
    void recolorRenderedItems();

private:
    void openFile();
    void processFile(const std::stop_token& stopToken, const QString& filePath);
    void releaseCurrentFile();
    void resetModel();
    void resetFilteredModel();
    bool isTextContainsFilter(const QString& text, const QRegularExpression& filter);
    bool startFromTheBeginningIfNeeded(bool force, QTextStream& stream, const QFile& file);
    void triggerRefiltering();
    void triggerRecoloring();
    void updateIndexLineWidth(bool force);
    int digitsInNumber(int value) const;
    QColor getColor(const QString& text, const QList<Settings::ColoringPattern>& patterns) const;
    QRegularExpression createRegExp(const QString& filter) const;

    template<typename DataType>
    void copyToClipBoard(const Utility::Models::SelectionListModel<DataType>& model) const;
    template<typename DataType>
    void copyToClipBoard(const std::set<int>& selection, const QList<DataType>& data) const;
    template<typename DataType>
    void deselect(Utility::Models::SelectionListModel<DataType>& model);
    template<typename DataType>
    void selectAll(Utility::Models::SelectionListModel<DataType>& model);

    Utility::FileSystemWatcher& m_fileWatcher;
    Utility::Models::SelectionListModel<LogLine> m_model;
    Utility::Models::SelectionListModel<FilteredLogLine> m_filteredModel;
    QString m_filter;
    QString m_filePath;
    QString m_fileName;

    std::mutex m_fileMutex;
    mutable std::shared_mutex m_filterMutex;
    std::jthread m_thread;
    std::condition_variable m_allowReading;
    std::atomic<bool> m_refilter = false;
    std::atomic<bool> m_recolor = false;
    std::atomic<bool> m_threadFinished = true;
    std::atomic<Settings::ColoringStrategy> m_coloringStrategy;
    bool m_forceOneThreadLoop = false;

    int m_fileSize = 0;
    int m_currentModelSize = 0;
    int m_indexLineWidth = Utility::Style::instance().indexLineWidth();
    int m_processedLineIndex = 0;
};

}
