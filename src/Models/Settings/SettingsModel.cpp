#include "SettingsModel.h"
#include <QGuiApplication>
#include <QFont>
#include "Formatter.h"

Models::Settings::SettingsModel::SettingsModel(QObject *parent) :
    QObject(parent),
    m_settings(QStringLiteral("Logger"), QStringLiteral("Settings"))
{
    updateLogLinesFont(m_settings.value(QStringLiteral("LogLinesFont"), QGuiApplication::font()).value<QFont>());
    m_coloringPatternsModel.pushBack({
        {":RQ :", QColor(Qt::magenta)},
        {":RP :", QColor(Qt::blue)},
        {":EV :", QColor(Qt::darkCyan)},
        {"WARN", QColor(QColorConstants::Svg::orange)},
        {"CRIT", QColor(Qt::red)},
        {"FATAL", QColor(Qt::darkRed)},
        {"MYLOG", QColor(Qt::darkGreen)},
        {"if1verbose", QColor(Qt::darkBlue)}
    });
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

void Models::Settings::SettingsModel::resetLogLinesFont()
{
    updateLogLinesFont(QGuiApplication::font());
}

QString Models::Settings::SettingsModel::formatFont(const QFont& font)
{
    return Utility::Formatter::formatFont(font);
}

void Models::Settings::SettingsModel::addPattern(const QString& filter, const QColor& color, bool caseSensitive)
{
    //TODO: add mechanism to save updates by save button?
    m_coloringPatternsModel.pushBack({filter, color, caseSensitive});
    emit coloringPatternsChanged();
}

const QList<Models::Settings::ColoringPattern> &Models::Settings::SettingsModel::coloringPatterns()
{
    return m_coloringPatternsModel.getRawData();
}

Utility::Models::ListModel<Models::Settings::ColoringPattern> *Models::Settings::SettingsModel::coloringPatternsModel()
{
    return &m_coloringPatternsModel;
}

