pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtCore
import Models
import Utility

Item {
    id: root

    height: tabBar.height

    readonly property alias currentIndex: tabBar.currentIndex
    required property OpenedFilesModel openedFilesModel

    function openNewFile() {
        fileDialog.open();
    }

    //TODO_LOW: Add scroll area to make mouse friendly
    TabBar {
        id: tabBar
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top
        Repeater {
            id: openedFilesRepeater
            model: root.openedFilesModel.model
            //TODO: Rework this delegate
            delegate: TabButton {
                id: openedFilesTabDelegate

                required property string name
                required property int index

                width: implicitContentWidth
                height: implicitContentHeight
                //rightPadding: closeButton.width + (Style.horizontalMargin * 2)
                text: "%1: %2".arg(openedFilesTabDelegate.index + 1).arg(openedFilesTabDelegate.name)

                contentItem: Item {
                    id: openedFilesTabDelegateContentItem
                    implicitWidth: textItem.width + closeButton.width + (Style.horizontalMargin * 3)
                    implicitHeight: textItem.height + (Style.verticalMargin * 2)

                    Text {
                        id: textItem
                        anchors.left: openedFilesTabDelegateContentItem.left
                        anchors.leftMargin: Style.horizontalMargin
                        anchors.verticalCenter: openedFilesTabDelegateContentItem.verticalCenter
                        text: openedFilesTabDelegate.text
                    }

                    MouseArea {
                        id: closeButton
                        anchors.left: textItem.right
                        anchors.top: openedFilesTabDelegateContentItem.top
                        anchors.bottom: openedFilesTabDelegateContentItem.bottom
                        anchors.leftMargin: Style.horizontalMargin
                        width: 30
                        onClicked: root.openedFilesModel.stopWatchingFile(openedFilesTabDelegate.index)

                        Text {
                            anchors.centerIn: closeButton
                            text: "x"
                            color: Style.closeButtonColor
                        }
                    }
                }



            }

            onItemAdded: function(index, item) {
                if (index === openedFilesRepeater.count - 1) {
                    tabBar.currentIndex = openedFilesRepeater.count - 1;
                }
            }
        }
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
