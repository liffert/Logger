#include "Style.h"

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



Utility::Style::Style(QObject* parent) : QObject(parent) {}
