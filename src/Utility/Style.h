#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QColor>

namespace Utility {

class Style : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QColor backgroundColor MEMBER m_backgroundColor CONSTANT)
    Q_PROPERTY(QColor textBackgroundColor MEMBER m_textBackgroundColor CONSTANT)
    Q_PROPERTY(QColor highlightColor MEMBER m_highlightColor CONSTANT)
    Q_PROPERTY(QColor brightTextColor MEMBER m_brightTextColor CONSTANT)
    Q_PROPERTY(QColor regularTextColor MEMBER m_regularTextColor CONSTANT)
    Q_PROPERTY(QColor colorPickerBorder MEMBER m_colorPickerBorder CONSTANT)
    Q_PROPERTY(int horizontalMargin MEMBER m_horizontalMargin CONSTANT)
    Q_PROPERTY(int verticalMargin MEMBER m_verticalMargin CONSTANT)
    Q_PROPERTY(int borderWidth MEMBER m_borderWidth CONSTANT)
    Q_PROPERTY(int filterHeight MEMBER m_filterHeight CONSTANT)

public:
    Q_DISABLE_COPY_MOVE(Style)
    static Style& instance();
    static Style* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

private:
    Style(QObject* parent = nullptr);

    const QColor m_backgroundColor{Qt::darkGray};
    const QColor m_textBackgroundColor{Qt::white};
    const QColor m_highlightColor{Qt::blue};
    const QColor m_brightTextColor{Qt::white};
    const QColor m_regularTextColor{Qt::black};
    const QColor m_colorPickerBorder{Qt::black};
    const int m_horizontalMargin = 8;
    const int m_verticalMargin = 4;
    const int m_borderWidth = 2;
    const int m_filterHeight = 30;
};

}
