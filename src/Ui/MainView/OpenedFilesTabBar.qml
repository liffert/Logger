pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtCore
import Utility


Item {
    id: root

    height: tabBar.height

    readonly property alias currentIndex: tabBar.currentIndex

    //Add scroll area to make mouse friendly
    TabBar {
        id: tabBar
        anchors.left: root.left
        anchors.top: root.top
        width: Math.min(tabBar.contentWidth, root.width - openFile.width)

        Repeater {
            id: openedFilesRepeater
            model: FileSystemWatcher.openedFilesModel
            delegate: TabButton {
                id: openedFilesTabDelegate

                required property var modelData
                required property int index

                width: openedFilesTabDelegate.implicitWidth + closeButton.width + 20
                text: openedFilesTabDelegate.modelData.name

                Button {
                    id: closeButton
                    text: "x"
                    anchors.right: openedFilesTabDelegate.right
                    anchors.top: openedFilesTabDelegate.top
                    anchors.bottom: openedFilesTabDelegate.bottom
                    onClicked: FileSystemWatcher.stopWatchingFile(openedFilesTabDelegate.index)
                }
            }
        }
    }

    Button {
        id: openFile
        anchors.top: tabBar.top
        anchors.left: tabBar.right
        anchors.bottom: tabBar.bottom
        text: "open file"

        onClicked: fileDialog.open()
    }

    FileDialog  {
        id: fileDialog
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        fileMode: FileDialog.OpenFile
        onAccepted: FileSystemWatcher.addFilePath(fileDialog.selectedFile)
    }
}
