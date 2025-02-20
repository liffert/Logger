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
        SplitView.minimumHeight: 40//set to delegate height

        contentWidth: Math.max(fileReaderModel.modelWidth + 10, fullFileView.width - fullFileView.effectiveScrollBarWidth)
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
            root.forceActiveFocus();
            fileReaderModel.updateItemSelection(item.index, exclusive, !item.isSelected || exclusive);
            internal.lastSelectedView = fullFileView;
        }
    }

    Item {
        id: filteredViewItem

        SplitView.fillWidth: true
        SplitView.preferredHeight: 300
        SplitView.minimumHeight: 40//Set to delegate height + filter height

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

        TextEdit {
            id: filter
            anchors.left: filterText.right
            anchors.right: filteredViewItem.right
            anchors.top: filteredViewItem.top
            height: filterText.height
        }

        TextView {
            id: filteredFileView

            anchors.left: filteredViewItem.left
            anchors.right: filteredViewItem.right
            anchors.top: filter.bottom
            anchors.bottom: filteredViewItem.bottom

            contentWidth: Math.max(fileReaderModel.filteredModelWidth + 10, filteredFileView.width - filteredFileView.effectiveScrollBarWidth)
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
        filter: filter.text
    }

    QtObject {
        id: internal
        property var lastSelectedView: null
    }
}
