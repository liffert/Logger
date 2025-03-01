pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtCore
import Models


Item {
    id: root

    height: tabBar.height

    readonly property alias currentIndex: tabBar.currentIndex
    required property OpenedFilesModel openedFilesModel

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
            //TODO: Rework this delegate
            delegate: TabButton {
                id: openedFilesTabDelegate

                required property string name
                required property int index

                width: openedFilesTabDelegate.implicitWidth + closeButton.width + 20
                text: "%1: %2".arg(openedFilesTabDelegate.index + 1).arg(openedFilesTabDelegate.name)

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

            onItemAdded: function(index, item) {
                if (index === openedFilesRepeater.count - 1) {
                    tabBar.currentIndex = openedFilesRepeater.count - 1;
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

    Connections {
        id: currentVisibleIndexUpdater
        target: tabBar
        enabled: false
        function onCurrentIndexChanged() {
            console.log("Test");
            root.openedFilesModel.currentVisibleIndex = tabBar.currentIndex;
        }
    }

    Component.onCompleted: {
        const indexToSet = root.openedFilesModel.currentVisibleIndex;
        if (indexToSet >= 0 && indexToSet < tabBar.count) {
            tabBar.currentIndex = indexToSet;
        }
        currentVisibleIndexUpdater.enabled = true;
    }
}
