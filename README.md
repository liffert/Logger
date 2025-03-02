# Logger

Log viewer application

# Requirements
Qt 6.5+
# Building and installing manually

1) Clone repo.
2) Create build folder and enter build folder
3) cmake pathToFolderWithRepo/Logger/src/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=pathToQtInstallationFolder/version/compiler -DCMAKE_INSTALL_PREFIX=pathToFolderForInstallation
4) cmake --build .

At this point your should have working application inside build folder which can simply be run by ./Logger.
If you want to have standalone application without dependencies on your current Qt install, proceed with installation process

5) cmake --install .

At this point you will have standalone application which can be run by ./pathToFolderForInstallation/bin/Logger
