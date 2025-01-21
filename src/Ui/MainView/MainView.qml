pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Models
import Utility
import Ui.Components

Item {
    id: root

    TabBar {
        id: tabBar
        anchors.left: root.left
        anchors.top: root.top
        width: Math.min(tabBar.contentWidth, root.width)

        Repeater {
            id: openedFilesRepeater
            model: FileSystemWatcher.openedFilesModel
            delegate: TabButton {
                id: openedFilesDelegate

                required property var modelData

                width: openedFilesDelegate.implicitWidth
                text: openedFilesDelegate.modelData
            }
        }

        TabButton {
            id: openFile
            text: "open file"

            onClicked: fileSelection.open()
        }
    }

    FileSelectionMenu {
        id: fileSelection
        anchors.top: tabBar.bottom
        anchors.left: root.left
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.topMargin: 20

        onFileAccepted: function(path) {
            fileReaderModel.openFile(path);
        }
    }

    TextView {
        id: scrollView
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: fileSelection.bottom
        anchors.topMargin: 20
        anchors.bottom: scrollView2.top
        anchors.bottomMargin: 50
        contentWidth: Math.max(fileReaderModel.modelWidth + 10, scrollView.width)
        model: fileReaderModel.model
    }

    TextView {
        id: scrollView2
        anchors.left: root.left
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: 300
        contentWidth: Math.max(fileReaderModel.filteredModelWidth + 10, scrollView.width)
        clip: true
        model: fileReaderModel.filteredModel
    }


    Rectangle {
        color: "white"
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: scrollView.bottom
        height: filterText.height
    }

    Text {
        id: filterText
        text: "CurrentFilter: "
        anchors.left: root.left
        anchors.top: filter.top
    }

    TextEdit {
        id: filter
        anchors.left: filterText.right
        anchors.right: root.right
        anchors.top: scrollView.bottom
        height: filterText.height
    }

    FileReaderModel {
        id: fileReaderModel
        filter: filter.text
    }
}
