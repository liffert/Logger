#include "FileReaderModel.h"

Models::FileReader::FileReaderModel::FileReaderModel(QObject *parent) : QObject(parent)
{
    qInfo() << __PRETTY_FUNCTION__;
}

void Models::FileReader::FileReaderModel::openFile(const QString &path)
{
    qInfo() << __PRETTY_FUNCTION__ << " " << path;
    m_path = path;
}
