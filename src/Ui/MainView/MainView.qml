import QtQuick
import Models

Item {
    id: root

    FileSelectionMenu {
        id: fileSelection
        anchors.centerIn: root
    }

    FileReaderModel {
        id: fileReaderModel
    }
}
