#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QList>
#include <QColor>
#include <QFont>
#include "Models/ListModel.h"
#include "Style.h"

namespace Models::Settings {

struct ColoringPattern {
    Q_GADGET
    Q_PROPERTY(QString pattern MEMBER pattern CONSTANT)
    Q_PROPERTY(QColor color MEMBER color CONSTANT)
    Q_PROPERTY(bool caseSensitive MEMBER caseSensitive CONSTANT)

public:
    QString pattern;
    QColor color = Utility::Style::instance().regularTextColor();
    bool caseSensitive = false;

    bool operator==(const ColoringPattern&) const = default;

    friend QDataStream& operator<<(QDataStream& stream, const ColoringPattern& object);
    friend QDataStream& operator>>(QDataStream& stream, ColoringPattern& object);
};

//TODO_LOW: store geometry of main window
class SettingsModel : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QFont logLinesFont READ logLinesFont NOTIFY logLinesFontChanged)
    Q_PROPERTY(Utility::Models::ListModel<ColoringPattern> *coloringPatternsModel READ coloringPatternsModel CONSTANT)

public:
    Q_DISABLE_COPY_MOVE(SettingsModel)
    static SettingsModel& instance();
    static SettingsModel* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    QFont logLinesFont() const;

    Q_INVOKABLE void updateLogLinesFont(const QFont& value);
    Q_INVOKABLE void resetLogLinesFont();
    //TODO_LOW: maybe make formatter class as singleton and use directly from qml
    Q_INVOKABLE QString formatFont(const QFont& font);
    Q_INVOKABLE void addPattern(const QString& filter, const QColor& color, bool caseSensitive);
    Q_INVOKABLE void deletePattern(int index);
    Q_INVOKABLE void updatePattern(const QString& filter, const QColor& color, bool caseSensitive, int index);
    Q_INVOKABLE void openSettings();
    Q_INVOKABLE void closeSettings();
    Q_INVOKABLE void moveColoringPattern(int from, int to);

    const QList<ColoringPattern>& coloringPatterns();
    Utility::Models::ListModel<ColoringPattern>* coloringPatternsModel();

signals:
    void logLinesFontChanged();
    void coloringPatternsChanged();

private:
    struct PersistentStorageKeys {
        static const QString COLORING_PATTERNS_KEY;
        static const QString LOG_LINES_FONT_KEY;
    };

    SettingsModel(QObject* parent = nullptr);
    ~SettingsModel();

    QSettings m_persistentStorage;
    QFont m_logLinesFont;
    Utility::Models::ListModel<ColoringPattern> m_coloringPatternsModel;
    QList<ColoringPattern> m_lastColoringPatterns;
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
    return stream;
}

}
