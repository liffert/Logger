#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QFile>
#include "Models/ListModel.h"

namespace Models::FileReader {

class FileReaderModel : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged)
    Q_PROPERTY(Utility::Models::ListModel<QString> *model READ model() FINAL CONSTANT)

public:
    FileReaderModel(QObject* parent = nullptr);

    Q_INVOKABLE void openFile(const QString& path);
    Utility::Models::ListModel<QString>* model();

signals:
    void textChanged();

private:
    QString m_path;
    QFile m_file;
    QString m_text;
    Utility::Models::ListModel<QString> m_model;
};

}
