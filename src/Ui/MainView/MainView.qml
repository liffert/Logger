pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtCore
import Models
import Utility
import Ui.Components

Item {
    id: root

    OpenedFilesTabBar {
        id: tabBar
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top
    }

    // Stack layout
    StackLayout {
        id: openedFiles

        anchors.left: root.left
        anchors.right: root.right
        anchors.top: tabBar.bottom
        anchors.bottom: root.bottom
        anchors.topMargin: 20

        currentIndex: tabBar.currentIndex

        Repeater {
            id: openedFilesViews
            model: FileSystemWatcher.openedFilesModel
            delegate: OpenedFileView {
                id: openedFileView

                required property var modelData

                filePath: modelData.path
            }
        }
    }

    ///

    //No opened files message
    Loader {
        anchors.fill: root
        active: openedFiles.count === 0
        sourceComponent: NoFileOpenedView {
            id: noFileOpened
            anchors.fill: parent
        }
    }

    ///

    //Helpers

    FileDialog  {
        id: fileDialog
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        fileMode: FileDialog.OpenFile
        onAccepted: FileSystemWatcher.addFilePath(fileDialog.selectedFile)
    }

    Shortcut {//To add Ctrl+O, Ctrl+shift+C
        context: Qt.ApplicationShortcut
        sequences: [StandardKey.Copy]
        onActivated: {
            openedFileView.copy();
        }
    }
}
