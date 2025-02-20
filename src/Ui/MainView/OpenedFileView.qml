pragma ComponentBehavior: Bound

import QtQuick

import Models

import Ui.Components

Item {
    id: root

    property alias filePath: fileReaderModel.filePath

    function copy() {
        //ToFix
        if (fullFileView.view.activeFocus) {
            fileReaderModel.copyToClipboardSelectedItems();
        } else {
            fileReaderModel.copyToClipboardSelectedFilteredItems();
        }
    }

    TextView {
        id: fullFileView
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top
        anchors.bottom: filteredFileView.top
        anchors.bottomMargin: 50
        contentWidth: Math.max(fileReaderModel.modelWidth + 10, fullFileView.width)
        model: fileReaderModel.model

        delegateComponent: TextViewDelegate {
            id: delegate

            required property var modelData

            text: delegate.modelData.text
            isSelected: delegate.modelData.selected
            lineIndexItemWidth: fileReaderModel.lineIndexItemWidth
        }

        onUpdateItemSelection: function(index, value) {
            fileReaderModel.updateItemSelection(index, false, value);
        }

        onItemSelected: function(item, exclusive) {
            filteredFileView.view.forceActiveFocus();
            fileReaderModel.updateItemSelection(item.index, exclusive, !item.isSelected || exclusive);
        }
    }

    TextView {
        id: filteredFileView
        anchors.left: root.left
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: 300
        contentWidth: Math.max(fileReaderModel.filteredModelWidth + 10, fullFileView.width)
        model: fileReaderModel.filteredModel

        delegateComponent: TextViewDelegate {
            id: delegate

            required property var modelData

            text: delegate.modelData.text
            isSelected: delegate.modelData.selected
            lineIndex: delegate.modelData.originalIndex
            lineIndexItemWidth: fileReaderModel.lineIndexItemWidth
        }

        onUpdateItemSelection: function(index, value) {
            fileReaderModel.updateFilteredItemSelection(index, false, value);
        }

        onItemSelected: function(item, exclusive) {
            filteredFileView.view.forceActiveFocus();
            fileReaderModel.updateFilteredItemSelection(item.index, exclusive, !item.isSelected || exclusive);
        }

        onItemDoubleClicked: function(item, exclusive) {
            fileReaderModel.updateFilteredItemSelection(item.lineIndex, exclusive, true);
            fileReaderModel.updateItemSelection(item.lineIndex, true, true);
            fullFileView.view.positionViewAtIndex(item.lineIndex, ListView.Center);
        }
    }


    Rectangle {
        color: "white"
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: fullFileView.bottom
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
        anchors.top: fullFileView.bottom
        height: filterText.height
    }

    FileReaderModel {
        id: fileReaderModel
        filter: filter.text
    }
}
