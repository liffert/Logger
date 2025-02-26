#pragma once
#include <QFileSystemWatcher>

namespace Utility {

class FileSystemWatcher : public QFileSystemWatcher {

public:
    static FileSystemWatcher& instance();

private:
    FileSystemWatcher(QObject* parent = nullptr);
};

}
