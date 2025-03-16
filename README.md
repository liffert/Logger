## License
Logger is licensed under the [GNU General Public License v3.0](LICENSE).  
You are free to use, modify, and distribute it under the terms of the GPLv3.  

See [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.html) for details.

# Logger

Log viewer application
ALPHA VERSION

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
