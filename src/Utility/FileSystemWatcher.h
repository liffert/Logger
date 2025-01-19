#pragma once
#include <QFileSystemWatcher>
#include <QObject>

namespace Utility {

class FileSystemWatcher : public QFileSystemWatcher {

public:
    static FileSystemWatcher& instance();

private:
    FileSystemWatcher(QObject* parent = nullptr);
};

}
