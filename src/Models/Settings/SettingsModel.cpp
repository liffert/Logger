#include "SettingsModel.h"
#include <QGuiApplication>
#include <QFont>
#include "Formatter.h"

Models::Settings::SettingsModel::SettingsModel(QObject *parent) :
    QObject(parent),
    m_settings(QStringLiteral("Logger"), QStringLiteral("Settings"))
{
    updateLogLinesFont(m_settings.value(QStringLiteral("LogLinesFont"), QGuiApplication::font()).value<QFont>());
    const auto patternsSize = m_settings.beginReadArray(QStringLiteral("ColoringPatterns"));
    for (int i = 0; i < patternsSize; i++) {
        m_settings.setArrayIndex(i);
        ColoringPattern pattern;
        pattern.pattern = m_settings.value(QStringLiteral("Pattern")).toString();
        pattern.color = m_settings.value(QStringLiteral("Color")).value<QColor>();
        pattern.caseSensitive = m_settings.value(QStringLiteral("CaseSensitive")).toBool();
        m_coloringPatternsModel.pushBack(pattern);
    }
    m_settings.endArray();
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
    m_coloringPatternsModel.pushBack({filter, color, caseSensitive});
}

void Models::Settings::SettingsModel::deletePattern(int index)
{
    m_coloringPatternsModel.remove(index);
}

void Models::Settings::SettingsModel::updatePattern(const QString& filter, const QColor& color, bool caseSensitive, int index)
{
    m_coloringPatternsModel.update(index, {filter, color, caseSensitive});
}

void Models::Settings::SettingsModel::openSettings()
{
    m_lastColoringPatterns = m_coloringPatternsModel.getRawData();
}

void Models::Settings::SettingsModel::closeSettings()
{
    const auto& coloringPatterns = m_coloringPatternsModel.getRawData();
    if (m_lastColoringPatterns != coloringPatterns || true) {
        emit coloringPatternsChanged();
        //m_settings.setValue(QStringLiteral("Test"), QVariant::fromValue(coloringPatterns));
        m_settings.beginWriteArray(QStringLiteral("ColoringPatterns"));
        for (int i = 0; i < coloringPatterns.size(); i++) {
            const auto& pattern = coloringPatterns.at(i);
            m_settings.setArrayIndex(i);
            m_settings.setValue(QStringLiteral("Pattern"), pattern.pattern);
            m_settings.setValue(QStringLiteral("Color"), pattern.color);
            m_settings.setValue(QStringLiteral("CaseSensitive"), pattern.caseSensitive);
        }
        m_settings.endArray();
        m_settings.sync();
    }
}

void Models::Settings::SettingsModel::moveColoringPattern(int from, int to)
{
    m_coloringPatternsModel.move(from, to);
}

const QList<Models::Settings::ColoringPattern>& Models::Settings::SettingsModel::coloringPatterns()
{
    return m_coloringPatternsModel.getRawData();
}

Utility::Models::ListModel<Models::Settings::ColoringPattern> *Models::Settings::SettingsModel::coloringPatternsModel()
{
    return &m_coloringPatternsModel;
}
