pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Models

import Ui.Components

SplitView {
    id: root

    orientation: Qt.Vertical

    property alias filePath: fileReaderModel.filePath

    function copy(copyAll) {
        if (copyAll) {
            fileReaderModel.copyAllItems();
        } else if (internal.lastSelectedView === fullFileView) {
            fileReaderModel.copyToClipboardSelectedItems();
        } else if (internal.lastSelectedView === filteredFileView) {
            fileReaderModel.copyToClipboardSelectedFilteredItems();
        }
    }

    function deselect() {
        if (internal.lastSelectedView === fullFileView) {
            fileReaderModel.deselectItems();
        } else if (internal.lastSelectedView === filteredFileView) {
            fileReaderModel.deselectFilteredItems();
        }
    }

    function selectAll() {
        if (internal.lastSelectedView === fullFileView) {
            fileReaderModel.selectAllItems();
        } else if (internal.lastSelectedView === filteredFileView) {
            fileReaderModel.selectAllFilteredItems();
        }
    }

    TextView {
        id: fullFileView

        SplitView.fillWidth: true
        SplitView.fillHeight: true
        SplitView.minimumHeight: 40//TODO: set to delegate height

        model: fileReaderModel.model

        delegateComponent: TextViewDelegate {
            id: delegate

            required property var modelData

            text: delegate.modelData.text
            isSelected: delegate.modelData.selected
            textColor: delegate.modelData.color
        }

        onUpdateItemsSelection: function(startIndex, endIndex, value) {
            fileReaderModel.updateItemsSelection(startIndex, endIndex, false, value);
        }

        onItemSelected: function(item, exclusive) {
            root.forceActiveFocus();
            fileReaderModel.updateItemSelection(item.index, exclusive, !item.isSelected || exclusive);
            internal.lastSelectedView = fullFileView;
        }
    }

    Item {
        id: filteredViewItem

        SplitView.fillWidth: true
        SplitView.preferredHeight: 300
        SplitView.minimumHeight: 40//TODO: Set to delegate height + filter height

        Rectangle {
            color: "white"
            anchors.left: filteredViewItem.left
            anchors.right: filteredViewItem.right
            anchors.top: filteredViewItem.top
            height: filterText.height
        }

        Text {
            id: filterText
            text: "CurrentFilter: "
            anchors.left: filteredViewItem.left
            anchors.top: filter.top
        }

        TextInput {
            id: filter
            anchors.left: filterText.right
            anchors.right: filteredViewItem.right
            anchors.top: filteredViewItem.top
            height: filterText.height
            onAccepted: fileReaderModel.filter = filter.displayText
        }

        TextView {
            id: filteredFileView

            anchors.left: filteredViewItem.left
            anchors.right: filteredViewItem.right
            anchors.top: filter.bottom
            anchors.bottom: filteredViewItem.bottom

            model: fileReaderModel.filteredModel

            delegateComponent: TextViewDelegate {
                id: delegate

                required property var modelData

                text: delegate.modelData.text
                isSelected: delegate.modelData.selected
                lineIndex: delegate.modelData.originalIndex
                textColor: delegate.modelData.color
            }

            onUpdateItemsSelection: function(startIndex, endIndex, value) {
                fileReaderModel.updateFilteredItemsSelection(startIndex, endIndex, false, value);
            }

            onItemSelected: function(item, exclusive) {
                root.forceActiveFocus();
                fileReaderModel.updateFilteredItemSelection(item.index, exclusive, !item.isSelected || exclusive);
                internal.lastSelectedView = filteredFileView;
            }

            onItemDoubleClicked: function(item, exclusive) {
                fileReaderModel.updateFilteredItemSelection(item.index, exclusive, true);
                fileReaderModel.updateItemSelection(item.lineIndex, true, true);
                fullFileView.view.positionViewAtIndex(item.lineIndex, ListView.Center);
            }
        }

    }

    FileReaderModel {
        id: fileReaderModel
        onItemsAdded: {
            fullFileView.view.positionViewAtEnd();
            filteredFileView.view.positionViewAtEnd();
        }
    }

    QtObject {
        id: internal
        property var lastSelectedView: null
    }
}
