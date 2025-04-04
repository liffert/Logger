/*
 * Logger - simple log viewer application.
 * Copyright (C) 2025 Dmytro Martyniuk <digitizenetwork@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "FileReaderModel.h"
#include <QRegularExpression>
#include <QGuiApplication>
#include <QClipboard>
#include <chrono>
#include <QFontMetrics>

Models::FileReader::FileReaderModel::FileReaderModel(QObject* parent) :
    QObject(parent),
    m_fileWatcher(Utility::FileSystemWatcher::instance()),
    m_coloringStrategy(Settings::SettingsModel::instance().coloringStrategy())
{
    connect(&m_fileWatcher, &Utility::FileSystemWatcher::fileChanged, this, [this](const auto& path) {
        if (path == m_filePath) {
            if (m_fileMutex.try_lock()) {
                m_forceOneThreadLoop = true;
                m_fileMutex.unlock();
                m_allowReading.notify_one();
            }
        }
    });

    connect(&Settings::SettingsModel::instance(), &Settings::SettingsModel::coloringPatternsChanged, this, &FileReaderModel::triggerRecoloring);
    connect(&Utility::Style::instance(), &Utility::Style::logLineFontChanged, this, [this]() {
        updateIndexLineWidth(true);
    });
    connect(&Settings::SettingsModel::instance(), &Settings::SettingsModel::coloringStrategyChanged, this, [this]() {
        m_coloringStrategy.store(Settings::SettingsModel::instance().coloringStrategy(), std::memory_order::relaxed);
        triggerRecoloring();
    });
    connect(this, &FileReaderModel::filterChanged, this, &FileReaderModel::triggerRefiltering);
    connect(this, &FileReaderModel::filePathChanged, this, &FileReaderModel::openFile);
}

Models::FileReader::FileReaderModel::~FileReaderModel()
{
    releaseCurrentFile();
}

void Models::FileReader::FileReaderModel::openFile()
{
    qInfo() << __PRETTY_FUNCTION__ << " " << m_filePath;
    if (!m_threadFinished.load(std::memory_order::relaxed)) {
        releaseCurrentFile();
    }

    if (m_filePath.isEmpty()) {
        qInfo() << __PRETTY_FUNCTION__ << " file path is empty";
        return;
    }

    m_threadFinished.store(false, std::memory_order::relaxed);
    m_thread = std::jthread([this, filePath = m_filePath](const std::stop_token& stopToken) {
        processFile(stopToken, filePath);
        m_threadFinished.store(true, std::memory_order::relaxed);
    });
}

QList<QByteArray> test;

void Models::FileReader::FileReaderModel::processFile(const std::stop_token& stopToken, const QString& filePath)
{
    using namespace std::chrono_literals;

    //TODO_LOW: option to set these values on UI?
    constexpr auto updateRate = 50ms;
    constexpr auto threadSleepThreashold = 2s;

    QList<Settings::ColoringPattern> coloringPatterns;
    QMetaObject::invokeMethod(this, [this]() {
        return Settings::SettingsModel::instance().coloringPatterns();
    }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(coloringPatterns), coloringPatterns));

    QFile file;
    file.setFileName(filePath);
    file.open(QIODevice::ReadOnly);

    auto currentFilter = createRegExp(filter());

    startFromTheBeginningIfNeeded(true, file);

    std::unique_lock lock(m_fileMutex);
    while (true) {
        m_allowReading.wait(lock, [this, &stopToken, &file]() {
            return (file.isOpen() && !file.atEnd()) || m_refilter.load(std::memory_order::relaxed) || m_recolor.load(std::memory_order::relaxed) || m_forceOneThreadLoop || stopToken.stop_requested();
        });

        if (m_forceOneThreadLoop) {
            m_forceOneThreadLoop = false;
        }

        qInfo() << "thread wake up: " << filePath;
        QList<LogLine> items;
        QList<FilteredLogLine> filteredItems;
        auto startReadingPoint = std::chrono::steady_clock::now();

        while (true) {
            if (stopToken.stop_requested()) {
                return;
            }

            while (m_recolor.load(std::memory_order::relaxed)) {
                if (stopToken.stop_requested()) {
                    return;
                }
                m_recolor.store(false, std::memory_order::relaxed);

                if(startFromTheBeginningIfNeeded(true, file)) {
                    items = {};
                    filteredItems = {};
                }

                QMetaObject::invokeMethod(this, [this, &coloringPatterns]() {
                    return Settings::SettingsModel::instance().coloringPatterns();
                }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(coloringPatterns), coloringPatterns));
            }

            while (m_refilter.load(std::memory_order::relaxed)) {
                if (stopToken.stop_requested()) {
                    return;
                }

                m_refilter.store(false, std::memory_order::relaxed);
                filteredItems = {};
                currentFilter = createRegExp(filter());
                QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);

                if (currentFilter.pattern().isEmpty()) {
                    break;
                }

                //Block to retreive full data
                //No performance issues with using copying because QList is implicitly shared container. What is
                //also provides safety mechanism in case if there for some reason will be a race condition and
                //original data gets modified.
                QList<LogLine> data;
                QMetaObject::invokeMethod(this, [this]() {
                    return m_model.getRawData();
                }, Qt::BlockingQueuedConnection, Q_RETURN_ARG(decltype(data), data));

                for (int i = 0; i < data.size(); i++) {
                    if (stopToken.stop_requested()) {
                        return;
                    }

                    if (m_refilter.load(std::memory_order::relaxed)) {
                        break;
                    }

                    if (startFromTheBeginningIfNeeded(false, file)) {
                        items = {};
                        filteredItems = {};
                        break;
                    }

                    const auto& item = data.at(i);
                    const auto& text = item.text;
                    if (isTextContainsFilter(text, currentFilter)) {
                        filteredItems.push_back({text, false, item.color, i});
                    }

                    const auto currentReadingPoint = std::chrono::steady_clock::now();
                    if ((currentReadingPoint - startReadingPoint > updateRate) && !filteredItems.isEmpty()) {
                        QMetaObject::invokeMethod(this, [this, filteredItems]() {
                            m_filteredModel.pushBack(filteredItems);
                            emit itemsAdded();
                        }, Qt::QueuedConnection);
                        startReadingPoint = currentReadingPoint;
                        filteredItems = {};
                    }
                }
            }

            if (startFromTheBeginningIfNeeded(false, file)) {
                items = {};
                filteredItems = {};
                //To make sure that when we are starting from the beginning new filter and coloring patterns will be used without overhead
                //if such request were made.
                continue;
            };

            if (!file.atEnd()) {
                const auto text = file.readLine();
                qInfo() << "MYLOG " << text << " " << text.isEmpty();
                if (!text.isEmpty()) {
                    const auto color = m_coloringStrategy == Settings::ColoringStrategy::ON_READ ? getColor(text, coloringPatterns) : Utility::Style::instance().regularTextColor();
                    items.push_back({.text = text, .color = color});
                    if (isTextContainsFilter(text, currentFilter)) {
                        filteredItems.push_back({text, false, color, m_currentModelSize});
                    }
                    m_currentModelSize++;
                }
            }

            //Update only with some rate. Sleep the thread only after threashold passed, because
            //there are circumstances under which fileChanged event will be fired with 1s interval
            //making thread refresh rate 1s as a result.
            const auto currentReadingPoint = std::chrono::steady_clock::now();
            const auto timePassed = currentReadingPoint - startReadingPoint;
            const auto triggerItemsAdded = !items.empty() || !filteredItems.empty();
            if (timePassed >= updateRate && triggerItemsAdded) {
                QMetaObject::invokeMethod(this, [this, items, filteredItems]() {
                    m_model.pushBack(items);
                    m_filteredModel.pushBack(filteredItems);
                    updateIndexLineWidth(false);
                    emit itemsAdded();
                }, Qt::QueuedConnection);
                items = {};
                filteredItems = {};
                startReadingPoint = currentReadingPoint;
            } else if (timePassed >= threadSleepThreashold && file.atEnd()) {
                qInfo() << "Thread sleep waiting for file update " << filePath;
                break;
            }
        }
    }
}

void Models::FileReader::FileReaderModel::updateItemSelection(int index, bool exclusive, bool value)
{
    m_model.updateSelection(index, exclusive, value);
}

void Models::FileReader::FileReaderModel::updateItemsSelection(int startIndex, int endIndex, bool exclusive, bool value)
{
    m_model.updateSelection(startIndex, endIndex, exclusive, value);
}

void Models::FileReader::FileReaderModel::updateFilteredItemSelection(int index, bool exclusive, bool value)
{
    m_filteredModel.updateSelection(index, exclusive, value);
}

void Models::FileReader::FileReaderModel::updateFilteredItemsSelection(int startIndex, int endIndex, bool exclusive, bool value)
{
    m_filteredModel.updateSelection(startIndex, endIndex, exclusive, value);
}

void Models::FileReader::FileReaderModel::selectAllItems()
{
    selectAll(m_model);
}

void Models::FileReader::FileReaderModel::selectAllFilteredItems()
{
    selectAll(m_filteredModel);
}

void Models::FileReader::FileReaderModel::deselectItems()
{
    deselect(m_model);
}

void Models::FileReader::FileReaderModel::deselectFilteredItems()
{
    deselect(m_filteredModel);
}

void Models::FileReader::FileReaderModel::copyToClipboardSelectedItems()
{
    copyToClipBoard(m_model);
}

void Models::FileReader::FileReaderModel::copyToClipboardSelectedFilteredItems()
{
    copyToClipBoard(m_filteredModel);
}

void Models::FileReader::FileReaderModel::copyAllItems()
{
    auto modelSelection = m_model.getSelection();
    const auto& filteredModelSelection = m_filteredModel.getSelection();
    const auto& filteredModelRawData = m_filteredModel.getRawData();

    for (const auto& selection : filteredModelSelection) {
        if (selection < 0 || selection >= filteredModelRawData.count()) {
            continue;
        }
        modelSelection.insert(filteredModelRawData.at(selection).originalIndex);
    }

    copyToClipBoard(modelSelection, m_model.getRawData());
}

QColor Models::FileReader::FileReaderModel::getColor(const QString& filter)
{
    return getColor(filter, Settings::SettingsModel::instance().coloringPatterns());
}

void Models::FileReader::FileReaderModel::setFilter(const QString& filter)
{
    std::unique_lock lock(m_filterMutex);
    if (m_filter != filter) {
        m_filter = filter;
        lock.unlock();
        emit filterChanged();
    }
}

const QString& Models::FileReader::FileReaderModel::filter() const
{
    std::shared_lock readLock(m_filterMutex);
    return m_filter;
}

Utility::Models::SelectionListModel<Models::FileReader::LogLine>* Models::FileReader::FileReaderModel::model()
{
    return &m_model;
}

Utility::Models::SelectionListModel<Models::FileReader::FilteredLogLine>* Models::FileReader::FileReaderModel::filteredModel()
{
    return &m_filteredModel;
}

//TODO check performance impact
bool Models::FileReader::FileReaderModel::startFromTheBeginningIfNeeded(bool force, QFile& file)
{
    const auto currentSize = file.size();
    const auto result = force || (currentSize < m_fileSize);
    if (result) {
        qInfo() << __PRETTY_FUNCTION__ << currentSize << " " << m_fileSize;
        QMetaObject::invokeMethod(this, &FileReaderModel::resetModel, Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, &FileReaderModel::resetFilteredModel, Qt::QueuedConnection);
        file.seek(0);
        m_currentModelSize = 0;
    }
    m_fileSize = currentSize;
    return result;
}

void Models::FileReader::FileReaderModel::triggerRefiltering()
{
    if (!m_refilter.load(std::memory_order::relaxed)) {
        m_refilter.store(true, std::memory_order::relaxed);
        m_allowReading.notify_one();
    }
}

void Models::FileReader::FileReaderModel::triggerRecoloring()
{
    if (m_coloringStrategy.load(std::memory_order::relaxed) == Settings::ColoringStrategy::ON_READ) {
        if (!m_recolor.load(std::memory_order::relaxed)) {
            m_recolor.store(true, std::memory_order::relaxed);
            m_allowReading.notify_one();
        }
    } else {
        emit recolorRenderedItems();
    }
}

void Models::FileReader::FileReaderModel::updateIndexLineWidth(bool force)
{
    const auto lastIndex = m_model.rowCount();
    const auto diff = digitsInNumber(lastIndex) - digitsInNumber(m_processedLineIndex);
    if (m_processedLineIndex != lastIndex) {
        m_processedLineIndex = lastIndex;
    }

    if (force || diff != 0) {
        QFontMetrics metrics(Utility::Style::instance().logLineFont());
        m_indexLineWidth = std::max(Utility::Style::instance().indexLineWidth(), metrics.horizontalAdvance(QString::number(lastIndex)));
        emit indexLineWidthChanged();
    }
}

int Models::FileReader::FileReaderModel::digitsInNumber(int value) const
{
    if (value == 0) {
        return 1;
    }
    return static_cast<int>(std::log10(std::abs(value))) + 1;
}

QColor Models::FileReader::FileReaderModel::getColor(const QString& text, const QList<Settings::ColoringPattern>& patterns) const
{
    for (const auto& pattern : patterns) {
        if (pattern.regexp.match(text).hasMatch()) {
            return pattern.color;
        }
    }
    return Utility::Style::instance().regularTextColor();
}

QRegularExpression Models::FileReader::FileReaderModel::createRegExp(const QString& filter) const
{
    return QRegularExpression(filter, QRegularExpression::CaseInsensitiveOption);
}

void Models::FileReader::FileReaderModel::releaseCurrentFile()
{
    m_thread.request_stop();
    m_allowReading.notify_one();
    //In case if thread stuck in blocking connection, process all events before this gets destroyed to exit from the thread properly.
    //Consired to use AllEvents to make sure that UI is not frozen if it takes too long to die. But as far as it should not, keep it like this for now.
    while (!m_threadFinished.load(std::memory_order::relaxed)) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    }
}

void Models::FileReader::FileReaderModel::resetModel()
{
    m_model.reset();
    emit modelReset();
}

void Models::FileReader::FileReaderModel::resetFilteredModel()
{
    m_filteredModel.reset();
    emit filteredModelReset();
}

bool Models::FileReader::FileReaderModel::isTextContainsFilter(const QString& text, const QRegularExpression& filter)
{
    return !filter.pattern().isEmpty() && filter.match(text).hasMatch();
}

template<typename DataType>
void Models::FileReader::FileReaderModel::copyToClipBoard(const Utility::Models::SelectionListModel<DataType>& model) const
{
    copyToClipBoard(model.getSelection(), model.getRawData());
}

template<typename DataType>
void Models::FileReader::FileReaderModel::copyToClipBoard(const std::set<int>& selection, const QList<DataType>& data) const
{
    if (selection.empty()) {
        return;
    }

    QStringList selectedItems;
    for (const auto& selection : selection) {
        if (selection < 0 || selection >= data.count()) {
            continue;
        }
        selectedItems << data.at(selection).text;
    }

    auto* clipboard = QGuiApplication::clipboard();
    clipboard->setText(selectedItems.join(QStringLiteral("\n")));
}

template<typename DataType>
void Models::FileReader::FileReaderModel::deselect(Utility::Models::SelectionListModel<DataType>& model)
{
    const auto selection = model.getSelection();

    for (const auto& selection : selection) {
        model.updateSelection(selection, false, false);
    }
}

template<typename DataType>
void Models::FileReader::FileReaderModel::selectAll(Utility::Models::SelectionListModel<DataType>& model)
{
    for (int i = 0; i < model.rowCount(); i++) {
        model.updateSelection(i, false, true);
    }
}
