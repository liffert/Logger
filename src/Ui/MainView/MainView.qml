import QtQuick
import Models

Item {
    id: root

    Text {
        id: fullText
        anchors.top: root.top
        anchors.left: root.left
        text: fileReaderModel.text
    }

    ListView {
        id: list
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: fullText.bottom
        anchors.topMargin: 20
        anchors.bottom: fileSelection.top
        model: fileReaderModel.model
        delegate: Rectangle {
            border.width: 2
            border.color: "black"
            width: list.width
            height: 100

            Text {
                anchors.centerIn: parent
                text: modelData
            }
        }
    }

    FileSelectionMenu {
        id: fileSelection
        anchors.fill: root
        anchors.topMargin: 500

        onFileAccepted: function(path) {
            fileReaderModel.openFile(path);
        }
    }

    FileReaderModel {
        id: fileReaderModel
    }
}
