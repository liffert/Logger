#pragma once
#include <QString>
#include <QFont>

namespace Utility {

class Formatter {

public:
    static QString formatFont(const QFont& font);

private:
    Formatter() = default;
};

}
