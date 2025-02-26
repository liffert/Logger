pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtCore


Item {
    id: root

    height: tabBar.height

    readonly property alias currentIndex: tabBar.currentIndex
    required property var openedFilesModel

    function openNewFile() {
        fileDialog.open();
    }

    //TODO: Add scroll area to make mouse friendly
    TabBar {
        id: tabBar
        anchors.left: root.left
        anchors.top: root.top
        width: Math.min(tabBar.contentWidth, root.width - openFile.width)

        Repeater {
            id: openedFilesRepeater
            model: root.openedFilesModel.model
            delegate: TabButton {
                id: openedFilesTabDelegate

                required property string name
                required property int index

                width: openedFilesTabDelegate.implicitWidth + closeButton.width + 20
                text: openedFilesTabDelegate.name

                Button {
                    id: closeButton
                    text: "x"
                    anchors.right: openedFilesTabDelegate.right
                    anchors.top: openedFilesTabDelegate.top
                    anchors.bottom: openedFilesTabDelegate.bottom
                    width: 20
                    onClicked: root.openedFilesModel.stopWatchingFile(openedFilesTabDelegate.index)
                }
            }

            //TODO: Do not do this action on initial openening, only on "openFile" button
            onItemAdded: function(index, item) {
                if (index === openedFilesRepeater.count - 1) {
                    tabBar.currentIndex = openedFilesRepeater.count - 1
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

        onClicked: root.openNewFile();
    }

    FileDialog  {
        id: fileDialog
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        fileMode: FileDialog.OpenFile
        onAccepted: root.openedFilesModel.addFilePath(fileDialog.selectedFile)
    }
}
