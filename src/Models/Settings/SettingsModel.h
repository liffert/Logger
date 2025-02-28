#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QList>
#include <QColor>
#include <QFont>

namespace Models::Settings {

struct ColoringPattern {
    QString pattern;
    QColor color = {Qt::black};
};

class SettingsModel : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QFont logLinesFont READ logLinesFont NOTIFY logLinesFontChanged)

public:
    Q_DISABLE_COPY_MOVE(SettingsModel)
    static SettingsModel& instance();
    static SettingsModel* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    QFont logLinesFont() const;

    Q_INVOKABLE void updateLogLinesFont(const QFont& value);
    Q_INVOKABLE void resetLogLinesFont();
    //TODO: check maybe for utility class formatter?
    Q_INVOKABLE QString formatFont(const QFont& font);
    QList<ColoringPattern> coloringPatterns();

signals:
    void logLinesFontChanged();
    void coloringPatternsChanged();

private:
    SettingsModel(QObject* parent = nullptr);

    QSettings m_settings;
    QFont m_logLinesFont;
    QList<ColoringPattern> m_coloringPatterns;
};

}
