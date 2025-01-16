import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtCore

Item {
    id: root

    signal fileAccepted(string path)

    FileDialog  {
        id: fileDialog
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        fileMode: FileDialog.OpenFile
        onAccepted: root.fileAccepted(fileDialog.selectedFile)
    }

    Button {
        id: openFileButton
        anchors.centerIn: root
        width: 200
        height: 200
        text: "Open file"
        onClicked: fileDialog.open()
    }
}
