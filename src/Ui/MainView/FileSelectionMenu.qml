import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtCore

Item {
    id: root
    height: openFileButton.height

    function open() {
        fileDialog.open();
    }

    signal fileAccepted(string path)

    FileDialog  {
        id: fileDialog
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        fileMode: FileDialog.OpenFile
        onAccepted: root.fileAccepted(fileDialog.selectedFile)
    }

    Button {
        id: openFileButton
        anchors.top: root.top
        anchors.left: root.left
        width: 200
        height: 100
        text: "Open file"
        onClicked: fileDialog.open()
    }

    Text {
        anchors.right: root.right
        anchors.verticalCenter: openFileButton.verticalCenter
        text: "Opened file: %1".arg(fileDialog.selectedFile)
    }
}
