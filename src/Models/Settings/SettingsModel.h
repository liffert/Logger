#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QList>
#include <QColor>
#include <QFont>
#include "Models/ListModel.h"

namespace Models::Settings {

struct ColoringPattern {
    Q_GADGET
    Q_PROPERTY(QString pattern MEMBER pattern CONSTANT)
    Q_PROPERTY(QColor color MEMBER color CONSTANT)
    Q_PROPERTY(bool caseSensitive MEMBER caseSensitive CONSTANT)

public:
    QString pattern;
    QColor color = {Qt::black};
    bool caseSensitive = false;

    bool operator==(const ColoringPattern&) const = default;
};

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
    //TODO: maybe make formatter class as singleton and use directly from qml
    Q_INVOKABLE QString formatFont(const QFont& font);
    Q_INVOKABLE void addPattern(const QString& filter, const QColor& color, bool caseSensitive);
    Q_INVOKABLE void openSettings();
    Q_INVOKABLE void closeSettings();

    const QList<ColoringPattern>& coloringPatterns();
    Utility::Models::ListModel<ColoringPattern>* coloringPatternsModel();

signals:
    void logLinesFontChanged();
    void coloringPatternsChanged();

private:
    SettingsModel(QObject* parent = nullptr);

    QSettings m_settings;
    QFont m_logLinesFont;
    Utility::Models::ListModel<ColoringPattern> m_coloringPatternsModel;
    QList<ColoringPattern> m_lastColoringPatterns;
};

}
