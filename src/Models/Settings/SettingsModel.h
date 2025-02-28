#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QList>
#include <QColor>
#include <QFont>
#include "Models/ListModel.h"

namespace Models::Settings {

struct ColoringPattern {
    Q_GADGET
    Q_PROPERTY(QString pattern MEMBER pattern CONSTANT)
    Q_PROPERTY(QColor color MEMBER color CONSTANT)

public:
    QString pattern;
    QColor color = {Qt::black};
};

class SettingsModel : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QFont logLinesFont READ logLinesFont NOTIFY logLinesFontChanged)
    Q_PROPERTY(Utility::Models::ListModel<ColoringPattern> *coloringPatternsModel READ coloringPatternsModel CONSTANT)

public:
    Q_DISABLE_COPY_MOVE(SettingsModel)
    static SettingsModel& instance();
    static SettingsModel* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    QFont logLinesFont() const;

    Q_INVOKABLE void updateLogLinesFont(const QFont& value);
    Q_INVOKABLE void resetLogLinesFont();
    //TODO: check maybe for utility class formatter?
    Q_INVOKABLE QString formatFont(const QFont& font);
    const QList<ColoringPattern>& coloringPatterns();
    Utility::Models::ListModel<ColoringPattern>* coloringPatternsModel();

signals:
    void logLinesFontChanged();
    void coloringPatternsChanged();

private:
    SettingsModel(QObject* parent = nullptr);

    QSettings m_settings;
    QFont m_logLinesFont;
    Utility::Models::ListModel<ColoringPattern> m_coloringPatternsModel;
};

}
