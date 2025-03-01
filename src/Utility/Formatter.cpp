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
