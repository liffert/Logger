/*
 * Logger - simple log viewer application.
 * Copyright (C) 2025 Dmytro Martyniuk <digitizenetwork@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QColor>
#include <QFont>

namespace Utility {

class Style : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QColor backgroundColor READ backgroundColor CONSTANT)
    Q_PROPERTY(QColor textBackgroundColor READ textBackgroundColor CONSTANT)
    Q_PROPERTY(QColor highlightColor READ highlightColor CONSTANT)
    Q_PROPERTY(QColor brightTextColor READ brightTextColor CONSTANT)
    Q_PROPERTY(QColor regularTextColor READ regularTextColor CONSTANT)
    Q_PROPERTY(QColor colorPickerBorder READ colorPickerBorder CONSTANT)
    Q_PROPERTY(QColor closeButtonColor READ closeButtonColor CONSTANT)
    Q_PROPERTY(QFont logLineFont READ logLineFont WRITE setLogLineFont NOTIFY logLineFontChanged)
    Q_PROPERTY(int horizontalMargin READ horizontalMargin CONSTANT)
    Q_PROPERTY(int verticalMargin READ verticalMargin CONSTANT)
    Q_PROPERTY(int borderWidth READ borderWidth CONSTANT)
    Q_PROPERTY(int filterHeight READ filterHeight CONSTANT)
    Q_PROPERTY(int indexLineWidth READ indexLineWidth CONSTANT)
    Q_PROPERTY(int logLineHeight READ logLineHeight NOTIFY logLineHeightChanged)

public:
    Q_DISABLE_COPY_MOVE(Style)
    static Style& instance();
    static Style* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

    const QColor& backgroundColor() const;
    const QColor& textBackgroundColor() const;
    const QColor& highlightColor() const;
    const QColor& brightTextColor() const;
    const QColor& regularTextColor() const;
    const QColor& colorPickerBorder() const;
    const QColor& closeButtonColor() const;
    const QFont& logLineFont() const;
    int horizontalMargin() const;
    int verticalMargin() const;
    int borderWidth() const;
    int filterHeight() const;
    int indexLineWidth() const;
    int logLineHeight() const;

    void setLogLineFont(const QFont& value);

signals:
    void logLineFontChanged();
    void logLineHeightChanged();

private:
    Style(QObject* parent = nullptr);
    void updateLogLineHeight();

    const QColor m_backgroundColor{Qt::darkGray};
    const QColor m_textBackgroundColor{Qt::white};
    const QColor m_highlightColor{Qt::blue};
    const QColor m_brightTextColor{Qt::white};
    const QColor m_regularTextColor{Qt::black};
    const QColor m_colorPickerBorder{Qt::black};
    const QColor m_closeButtonColor{Qt::red};
    const int m_horizontalMargin = 8;
    const int m_verticalMargin = 4;
    const int m_borderWidth = 2;
    const int m_filterHeight = 30;
    const int m_indexLineWidth = 50;

    QFont m_logLineFont;
    int m_logLineHeight = 20;

};

}
