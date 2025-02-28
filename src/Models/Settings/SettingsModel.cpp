#include "SettingsModel.h"
#include <QGuiApplication>
#include <QFont>
#include <QFontDatabase>

Models::Settings::SettingsModel::SettingsModel(QObject *parent) :
    QObject(parent),
    m_settings(QStringLiteral("Logger"), QStringLiteral("Settings"))
{
    updateLogLinesFont(m_settings.value(QStringLiteral("LogLinesFont"), QGuiApplication::font()).value<QFont>());
}

Models::Settings::SettingsModel* Models::Settings::SettingsModel::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);

    auto& objectInstance = instance();
    QJSEngine::setObjectOwnership(&objectInstance, QJSEngine::CppOwnership);
    return &instance();
}

QFont Models::Settings::SettingsModel::logLinesFont() const
{
    return m_logLinesFont;
}

Models::Settings::SettingsModel &Models::Settings::SettingsModel::instance()
{
    static SettingsModel instance;
    return instance;
}

void Models::Settings::SettingsModel::updateLogLinesFont(const QFont& value)
{
    if (m_logLinesFont != value) {
        m_logLinesFont = value;
        m_settings.setValue(QStringLiteral("LogLinesFont"), value);
        emit logLinesFontChanged();
    }
}

