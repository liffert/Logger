#include "Style.h"
#include <QFontMetrics>

Utility::Style::Style(QObject* parent) : QObject(parent) {
    connect(this, &Style::logLineFontChanged, this, &Style::updateLogLineHeight);
    updateLogLineHeight();
}

Utility::Style& Utility::Style::instance()
{
    static Style instance;
    return instance;
}

Utility::Style* Utility::Style::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);

    auto& objectInstance = instance();
    QJSEngine::setObjectOwnership(&objectInstance, QJSEngine::CppOwnership);
    return &instance();
}

const QColor &Utility::Style::backgroundColor() const
{
    return m_backgroundColor;
}

const QColor& Utility::Style::textBackgroundColor() const
{
    return m_textBackgroundColor;
}

const QColor& Utility::Style::highlightColor() const
{
    return m_highlightColor;
}

const QColor& Utility::Style::brightTextColor() const
{
    return m_brightTextColor;
}

const QColor& Utility::Style::regularTextColor() const
{
    return m_regularTextColor;
}

const QColor& Utility::Style::colorPickerBorder() const
{
    return m_colorPickerBorder;
}

const QColor& Utility::Style::closeButtonColor() const
{
    return m_closeButtonColor;
}

const QFont& Utility::Style::logLineFont() const
{
    return m_logLineFont;
}

int Utility::Style::horizontalMargin() const
{
    return m_horizontalMargin;
}

int Utility::Style::verticalMargin() const
{
    return m_verticalMargin;
}

int Utility::Style::borderWidth() const
{
    return m_borderWidth;
}

int Utility::Style::filterHeight() const
{
    return m_filterHeight;
}

int Utility::Style::indexLineWidth() const
{
    return m_indexLineWidth;
}

int Utility::Style::logLineHeight() const
{
    return m_logLineHeight;
}

void Utility::Style::setLogLineFont(const QFont& value)
{
    if (m_logLineFont != value) {
        m_logLineFont = value;
        emit logLineFontChanged();
    }
}

void Utility::Style::updateLogLineHeight()
{
    constexpr int defaultHeight = 20;
    QFontMetrics metrics(m_logLineFont);
    const auto newHeight = std::max(defaultHeight, metrics.height());
    if (m_logLineHeight != newHeight) {
        m_logLineHeight = newHeight;
        emit logLineHeightChanged();
    }
}
