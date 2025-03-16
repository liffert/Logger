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

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QList>
#include <QColor>
#include <QFont>
#include "Models/ListModel.h"
#include "Style.h"
#include <QRegularExpression>

namespace Models::Settings {
QML_ELEMENT
Q_NAMESPACE

enum class ColoringStrategy {
    ON_READ,
    ON_RENDER
};
Q_ENUM_NS(ColoringStrategy)

struct ColoringPattern {
    Q_GADGET
    Q_PROPERTY(QString pattern MEMBER pattern CONSTANT)
    Q_PROPERTY(QColor color MEMBER color CONSTANT)
    Q_PROPERTY(bool caseSensitive MEMBER caseSensitive CONSTANT)

public:
    QString pattern;
    QColor color = Utility::Style::instance().regularTextColor();
    bool caseSensitive = false;
    QRegularExpression regexp;

    bool operator==(const ColoringPattern&) const = default;

    friend QDataStream& operator<<(QDataStream& stream, const ColoringPattern& object);
    friend QDataStream& operator>>(QDataStream& stream, ColoringPattern& object);
};

//TODO_LOW: store geometry of main window
class SettingsModel : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(Utility::Models::ListModel<ColoringPattern> *coloringPatternsModel READ coloringPatternsModel CONSTANT)
    Q_PROPERTY(ColoringStrategy coloringStrategy READ coloringStrategy WRITE setColoringStrategy NOTIFY coloringStrategyChanged)

public:
    Q_DISABLE_COPY_MOVE(SettingsModel)
    static SettingsModel& instance();
    static SettingsModel* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

    Q_INVOKABLE void updateLogLineFont(const QFont& value);
    Q_INVOKABLE void resetLogLineFont();
    //TODO_LOW: maybe make formatter class as singleton and use directly from qml
    Q_INVOKABLE QString formatFont(const QFont& font);
    Q_INVOKABLE void addPattern(const QString& filter, const QColor& color, bool caseSensitive);
    Q_INVOKABLE void deletePattern(int index);
    Q_INVOKABLE void updatePattern(const QString& filter, const QColor& color, bool caseSensitive, int index);
    Q_INVOKABLE void openSettings();
    Q_INVOKABLE void closeSettings();
    Q_INVOKABLE void moveColoringPattern(int from, int to);
    ColoringStrategy coloringStrategy() const;
    void setColoringStrategy(ColoringStrategy value);

    const QList<ColoringPattern>& coloringPatterns();
    Utility::Models::ListModel<ColoringPattern>* coloringPatternsModel();

signals:
    void coloringPatternsChanged();
    void coloringStrategyChanged();

private:
    struct PersistentStorageKeys {
        static const QString COLORING_PATTERNS_KEY;
        static const QString LOG_LINES_FONT_KEY;
        static const QString COLORING_STRATEGY_KEY;
    };

    SettingsModel(QObject* parent = nullptr);
    ~SettingsModel();
    void invokeColoringPatternsChanged(bool onClose);

    QSettings m_persistentStorage;
    Utility::Models::ListModel<ColoringPattern> m_coloringPatternsModel;
    QList<ColoringPattern> m_lastColoringPatterns;
    ColoringStrategy m_coloringStrategy = ColoringStrategy::ON_READ;
};

inline QDataStream& operator<<(QDataStream& stream, const ColoringPattern& object)
{
    stream << object.pattern;
    stream << object.color;
    stream << object.caseSensitive;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, ColoringPattern& object)
{
    stream >> object.pattern;
    stream >> object.color;
    stream >> object.caseSensitive;

    //TODO: Global create regexp function
    QRegularExpression regexp(object.pattern, object.caseSensitive ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption);
    object.regexp = regexp;

    return stream;
}

}
