pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtCore
import Models
import Utility
import Ui.Components

Item {
    id: root

    TabBar {
        id: tabBar
        anchors.left: root.left
        anchors.top: root.top
        width: Math.min(tabBar.contentWidth, root.width - openFile.width)

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
    }

    Button {
        id: openFile
        anchors.top: tabBar.top
        anchors.left: tabBar.right
        anchors.bottom: tabBar.bottom
        text: "open file"

        onClicked: fileDialog.open()
    }

    TextView {
        id: scrollView
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: tabBar.bottom
        anchors.topMargin: 20
        anchors.bottom: scrollView2.top
        anchors.bottomMargin: 50
        contentWidth: Math.max(fileReaderModel.modelWidth + 10, scrollView.width)
        model: fileReaderModel.model

        onCopySelection: fileReaderModel.copyToClipboardSelectedItems()

        delegateComponent: TextViewDelegate {
            id: delegate

            required property var modelData
            text: delegate.modelData.text
            selected: delegate.modelData.selected

            onSelected: function(exclusive){
                scrollView.view.forceActiveFocus();
                fileReaderModel.selectItem(delegate.index, exclusive);
                internal.lastTextViewSelection = scrollView;
            }
        }
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

        onCopySelection: fileReaderModel.copyToClipboardSelectedFilteredItems()

        delegateComponent: TextViewDelegate {
            id: delegate

            required property var modelData
            text: delegate.modelData.text
            selected: delegate.modelData.selected
            lineIndex: delegate.modelData.originalIndex

            onSelected: function(exclusive){
                scrollView2.view.forceActiveFocus();
                fileReaderModel.selectFilteredItem(delegate.index, exclusive);
                internal.lastTextViewSelection = scrollView2;
            }
            onDoubleClicked: {
                fileReaderModel.selectItem(delegate.lineIndex, true);
                scrollView.view.positionViewAtIndex(delegate.lineIndex, ListView.Center);
            }
        }
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

    Loader {
        anchors.fill: root
        active: FileSystemWatcher.openedFilesModel.rowCount === 0//fix
        sourceComponent: NoFileOpenedView {
            id: noFileOpened
            anchors.fill: parent
        }
    }

    FileDialog  {
        id: fileDialog
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        fileMode: FileDialog.OpenFile
        onAccepted: fileReaderModel.openFile(fileDialog.selectedFile)
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequences: [StandardKey.Copy]
        onActivated: {
            if (Boolean(internal.lastTextViewSelection)) {
                internal.lastTextViewSelection.copySelection();
            }
        }
    }

    FileReaderModel {
        id: fileReaderModel
        filter: filter.text
    }

    QtObject {
        id: internal

        property var lastTextViewSelection: null
    }
}
