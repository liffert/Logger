#include "SettingsModel.h"
#include <QGuiApplication>
#include <QFont>
#include "Formatter.h"

const QString Models::Settings::SettingsModel::PersistentStorageKeys::COLORING_PATTERNS_KEY = QStringLiteral("ColoringPatterns");
const QString Models::Settings::SettingsModel::PersistentStorageKeys::LOG_LINES_FONT_KEY = QStringLiteral("LogLineFont");
const QString Models::Settings::SettingsModel::PersistentStorageKeys::COLORING_STRATEGY_KEY = QStringLiteral("ColoringStrategy");

Models::Settings::SettingsModel::SettingsModel(QObject *parent) :
    QObject(parent),
    m_persistentStorage(QStringLiteral("Logger"), QStringLiteral("Settings"))
{
    //Register for properly working QSettings serialization
    qRegisterMetaType<QList<ColoringPattern>>();
    qRegisterMetaType<ColoringStrategy>();

    updateLogLineFont(m_persistentStorage.value(Models::Settings::SettingsModel::PersistentStorageKeys::LOG_LINES_FONT_KEY, QGuiApplication::font()).value<QFont>());
    setColoringStrategy(m_persistentStorage.value(Models::Settings::SettingsModel::PersistentStorageKeys::COLORING_STRATEGY_KEY, QVariant::fromValue(ColoringStrategy::ON_READ)).value<ColoringStrategy>());
    m_coloringPatternsModel.pushBack(m_persistentStorage.value(Models::Settings::SettingsModel::PersistentStorageKeys::COLORING_PATTERNS_KEY).value<QList<ColoringPattern>>());
}

Models::Settings::SettingsModel::~SettingsModel()
{
    m_persistentStorage.setValue(Models::Settings::SettingsModel::PersistentStorageKeys::COLORING_PATTERNS_KEY, QVariant::fromValue(m_coloringPatternsModel.getRawData()));
    m_persistentStorage.setValue(Models::Settings::SettingsModel::PersistentStorageKeys::LOG_LINES_FONT_KEY, Utility::Style::instance().logLineFont());
    m_persistentStorage.setValue(Models::Settings::SettingsModel::PersistentStorageKeys::COLORING_STRATEGY_KEY, QVariant::fromValue(m_coloringStrategy));
}

void Models::Settings::SettingsModel::invokeColoringPatternsChanged(bool onClose)
{
    //TODO: recheck
    const auto& currentColoringPatterns = m_coloringPatternsModel.getRawData();
    if (m_coloringStrategy == ColoringStrategy::ON_RENDER || (onClose && m_lastColoringPatterns != currentColoringPatterns)) {
        m_lastColoringPatterns = currentColoringPatterns;
        emit coloringPatternsChanged();
    }
}

Models::Settings::SettingsModel* Models::Settings::SettingsModel::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);

    auto& objectInstance = instance();
    QJSEngine::setObjectOwnership(&objectInstance, QJSEngine::CppOwnership);
    return &instance();
}

Models::Settings::SettingsModel &Models::Settings::SettingsModel::instance()
{
    static SettingsModel instance;
    return instance;
}

void Models::Settings::SettingsModel::updateLogLineFont(const QFont& value)
{
    Utility::Style::instance().setLogLineFont(value);
}

void Models::Settings::SettingsModel::resetLogLineFont()
{
    updateLogLineFont(QGuiApplication::font());
}

QString Models::Settings::SettingsModel::formatFont(const QFont& font)
{
    return Utility::Formatter::formatFont(font);
}

void Models::Settings::SettingsModel::addPattern(const QString& filter, const QColor& color, bool caseSensitive)
{
    QRegularExpression regexp(filter, caseSensitive ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption);
    m_coloringPatternsModel.pushBack({filter, color, caseSensitive, regexp});
    invokeColoringPatternsChanged(false);
}

void Models::Settings::SettingsModel::deletePattern(int index)
{
    m_coloringPatternsModel.remove(index);
    invokeColoringPatternsChanged(false);
}

void Models::Settings::SettingsModel::updatePattern(const QString& filter, const QColor& color, bool caseSensitive, int index)
{
    QRegularExpression regexp(filter, caseSensitive ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption);
    m_coloringPatternsModel.update(index, {filter, color, caseSensitive, regexp});
    invokeColoringPatternsChanged(false);
}

void Models::Settings::SettingsModel::openSettings()
{
    m_lastColoringPatterns = m_coloringPatternsModel.getRawData();
}

void Models::Settings::SettingsModel::closeSettings()
{
    invokeColoringPatternsChanged(true);
}

void Models::Settings::SettingsModel::moveColoringPattern(int from, int to)
{
    m_coloringPatternsModel.move(from, to);
    invokeColoringPatternsChanged(false);
}

Models::Settings::ColoringStrategy Models::Settings::SettingsModel::coloringStrategy() const
{
    return m_coloringStrategy;
}

void Models::Settings::SettingsModel::setColoringStrategy(ColoringStrategy value)
{
    if (m_coloringStrategy != value) {
        m_coloringStrategy = value;
        emit coloringStrategyChanged();
    }
}

const QList<Models::Settings::ColoringPattern>& Models::Settings::SettingsModel::coloringPatterns()
{
    return m_coloringPatternsModel.getRawData();
}

Utility::Models::ListModel<Models::Settings::ColoringPattern> *Models::Settings::SettingsModel::coloringPatternsModel()
{
    return &m_coloringPatternsModel;
}
