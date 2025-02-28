#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QList>
#include <QFontDatabase>

namespace Models::Settings {

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

signals:
    void logLinesFontChanged();

private:
    SettingsModel(QObject* parent = nullptr);

    QSettings m_settings;
    QFont m_logLinesFont;
};

}
