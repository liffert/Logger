import QtQuick
import Models

Item {
    id: root

    FileSelectionMenu {
        id: fileSelection
        anchors.fill: root

        onFileAccepted: function(path) {
            fileReaderModel.openFile(path);
        }
    }

    FileReaderModel {
        id: fileReaderModel
    }
}
