pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Models

import Ui.Components

SplitView {
    id: root

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

    function toggleAutoScroll() {
        if (internal.lastSelectedView === fullFileView) {
            fullFileView.toolbar.autoScrollEnabled = !fullFileView.toolbar.autoScrollEnabled;
        } else if (internal.lastSelectedView === filteredFileView) {
            filteredFileView.toolbar.autoScrollEnabled = !filteredFileView.toolbar.autoScrollEnabled;
        } else {
            fullFileView.toolbar.autoScrollEnabled = !fullFileView.toolbar.autoScrollEnabled;
            filteredFileView.toolbar.autoScrollEnabled = !filteredFileView.toolbar.autoScrollEnabled;
        }
    }

    orientation: Qt.Vertical
    handle: Rectangle {
        implicitHeight: 10
        color: SplitHandle.pressed ? Qt.darker("white", 1.2)
                                   : SplitHandle.hovered ? Qt.darker("white", 1.1)
                                                         : "white"
    }

    TextView {
        id: fullFileView

        SplitView.fillWidth: true
        SplitView.fillHeight: true
        SplitView.minimumHeight: 100

        model: fileReaderModel.model

        delegateComponent: TextViewDelegate {
            id: fullFileViewDelegate

            required property var modelData

            text: fullFileViewDelegate.modelData.text
            isSelected: fullFileViewDelegate.modelData.selected
            textColor: fullFileViewDelegate.modelData.color
        }

        onUpdateItemsSelection: function(startIndex, endIndex, value) {
            fileReaderModel.updateItemsSelection(startIndex, endIndex, false, value);
        }

        onItemSelected: function(item, exclusive) {
            root.forceActiveFocus();
            fileReaderModel.updateItemSelection(item.index, exclusive, !item.isSelected || exclusive);
            internal.lastSelectedView = fullFileView;
        }

        Connections {
            target: fileReaderModel
            enabled: fullFileView.toolbar.autoScrollEnabled
            function onItemsAdded() {
                if (fullFileView.toolbar.autoScrollEnabled) {
                    fullFileView.view.positionViewAtEnd();
                }
            }
        }
    }

    TextView {
        id: filteredFileView

        SplitView.fillWidth: true
        SplitView.preferredHeight: 300
        SplitView.minimumHeight: 100

        model: fileReaderModel.filteredModel
        toolbar.showFilter: true

        delegateComponent: TextViewDelegate {
            id: filteredFileViewDelegate

            required property var modelData

            text: filteredFileViewDelegate.modelData.text
            isSelected: filteredFileViewDelegate.modelData.selected
            lineIndex: filteredFileViewDelegate.modelData.originalIndex
            textColor: filteredFileViewDelegate.modelData.color
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
            fullFileView.toolbar.autoScrollEnabled = false;
            fileReaderModel.updateFilteredItemSelection(item.index, exclusive, true);
            fileReaderModel.updateItemSelection(item.lineIndex, true, true);
            fullFileView.view.positionViewAtIndex(item.lineIndex, ListView.Center);
        }

        Connections {
            target: fileReaderModel
            enabled: filteredFileView.toolbar.autoScrollEnabled
            function onItemsAdded() {
                if (filteredFileView.toolbar.autoScrollEnabled) {
                    filteredFileView.view.positionViewAtEnd();
                }
            }
        }
    }

    Connections {
        target: filteredFileView.toolbar
        function onProcessFilter(filterText) {
            fileReaderModel.filter = filterText;
        }
    }

    FileReaderModel {
        id: fileReaderModel

        onModelReset: fullFileView.view.contentWidth = fullFileView.view.width
        onFilteredModelReset: filteredFileView.view.contentWidth = filteredFileView.view.width
    }

    QtObject {
        id: internal
        property var lastSelectedView: null
    }
}
