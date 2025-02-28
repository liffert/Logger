#include "SettingsModel.h"
#include <QGuiApplication>
#include <QFont>
#include <QFontDatabase>

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
    QString result = font.family();
    const auto fontWeight = font.weight();
    if (fontWeight >= QFont::Black) {
        result.append(QStringLiteral(" Black"));
    } else if (fontWeight >= QFont::ExtraBold) {
        result.append(QStringLiteral(" Extra Bold"));
    } else if (fontWeight >= QFont::Bold) {
        result.append(QStringLiteral(" Bold"));
    } else if (fontWeight >= QFont::DemiBold) {
        result.append(QStringLiteral(" Demi Bold"));
    } else if (fontWeight >= QFont::Medium) {
        result.append(QStringLiteral(" Medium"));
    } else if (fontWeight >= QFont::Normal) {
        result.append(QStringLiteral(" Regular"));
    } else if (fontWeight >= QFont::Light) {
        result.append(QStringLiteral(" Light"));
    } else if (fontWeight >= QFont::Thin) {
        result.append(QStringLiteral(" Thin"));
    }

    if (font.italic()) {
        result.append(QStringLiteral(" Italic"));
    }

    result.append(QStringLiteral(" %1").arg(QString::number(font.pointSize())));
    return result;
}

const QList<Models::Settings::ColoringPattern> &Models::Settings::SettingsModel::coloringPatterns()
{
    return m_coloringPatternsModel.getRawData();
}

Utility::Models::ListModel<Models::Settings::ColoringPattern> *Models::Settings::SettingsModel::coloringPatternsModel()
{
    return &m_coloringPatternsModel;
}

