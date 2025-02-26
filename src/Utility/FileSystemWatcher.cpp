#include "FileSystemWatcher.h"

Utility::FileSystemWatcher& Utility::FileSystemWatcher::instance()
{
    static FileSystemWatcher instance;
    return instance;
}

Utility::FileSystemWatcher::FileSystemWatcher(QObject* parent) : QFileSystemWatcher(parent) {}
