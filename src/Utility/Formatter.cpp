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

#include "Formatter.h"

QString Utility::Formatter::formatFont(const QFont& font)
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
