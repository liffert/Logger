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

Utility::Style::Style(QObject* parent) : QObject(parent) {}
