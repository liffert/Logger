import QtQuick
import QtQuick.Controls
import Models

Item {
    id: root

    FileSelectionMenu {
        id: fileSelection
        anchors.top: root.top
        anchors.left: root.left
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.topMargin: 20

        onFileAccepted: function(path) {
            fileReaderModel.openFile(path);
        }
    }

    ScrollView {
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: fileSelection.bottom
        anchors.topMargin: 20
        anchors.bottom: root.bottom
        clip: true

        ListView {
            id: list
            model: fileReaderModel.model

            delegate: Rectangle {
                border.width: 2
                border.color: "black"
                width: list.width
                height: textItem.height + 10

                Text {
                    id: textItem
                    anchors.centerIn: parent
                    text: modelData
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        console.log("MYLOG");
                    }
                }
            }
        }
    }

    FileReaderModel {
        id: fileReaderModel
    }
}
