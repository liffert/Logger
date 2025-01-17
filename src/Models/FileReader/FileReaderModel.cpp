#include "FileReaderModel.h"

Models::FileReader::FileReaderModel::FileReaderModel(QObject *parent) : QObject(parent)
{
    qInfo() << __PRETTY_FUNCTION__;
}

void Models::FileReader::FileReaderModel::openFile(const QString &path)
{
    qInfo() << __PRETTY_FUNCTION__ << " " << path;
    m_path = path;
    m_file.setFileName(QUrl(path).toLocalFile());
    m_file.open(QIODevice::ReadOnly);

    QTextStream stream(&m_file);

    m_model.reset();
    int index = 0;
    while (!stream.atEnd()) {
        m_model.insert(index, stream.readLine());
        index++;
    }

    stream.seek(0);
    m_text = stream.readAll();
    emit textChanged();
}

Utility::Models::ListModel<QString> *Models::FileReader::FileReaderModel::model()
{
    return &m_model;
}
