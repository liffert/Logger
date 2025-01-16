#pragma once

#include <QObject>
#include <QQmlEngine>

namespace Models::FileReader {

class FileReaderModel : public QObject {
    Q_OBJECT
    QML_ELEMENT
public:
    FileReaderModel(QObject* parent = nullptr);
};

}
