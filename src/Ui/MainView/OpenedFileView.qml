pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Models

import Ui.Components

SplitView {
    id: root

    property alias filterText: fileReaderModel.filter
    property alias filePath: fileReaderModel.filePath
    property alias fullFileViewAutoScrollEnabled: fullFileView.autoScrollEnabled
    property alias filteredFileViewAutoScrollEnabled: filteredFileView.autoScrollEnabled

    signal processFilter(var filterText)

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
            fullFileView.autoScrollEnabled = !fullFileView.autoScrollEnabled;
        } else if (internal.lastSelectedView === filteredFileView) {
            filteredFileView.autoScrollEnabled = !filteredFileView.autoScrollEnabled;
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
        SplitView.minimumHeight: fullFileView.toolbar.height + 50

        model: fileReaderModel.model

        delegateComponent: TextViewDelegate {
            id: fullFileViewDelegate

            required property var modelData

            text: fullFileViewDelegate.modelData.text
            isSelected: fullFileViewDelegate.modelData.selected
            textColor: fullFileViewDelegate.modelData.color
            textFont: SettingsModel.logLinesFont
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
            enabled: fullFileView.autoScrollEnabled
            function onItemsAdded() {
                if (fullFileView.autoScrollEnabled) {
                    fullFileView.view.positionViewAtEnd();
                }
            }
        }
    }

    TextView {
        id: filteredFileView

        SplitView.fillWidth: true
        SplitView.preferredHeight: filteredFileView.toolbar.height + 300
        SplitView.minimumHeight: filteredFileView.toolbar.height + 200

        model: fileReaderModel.filteredModel
        toolbar.showFilter: true

        delegateComponent: TextViewDelegate {
            id: filteredFileViewDelegate

            required property var modelData

            text: filteredFileViewDelegate.modelData.text
            isSelected: filteredFileViewDelegate.modelData.selected
            lineIndex: filteredFileViewDelegate.modelData.originalIndex
            textColor: filteredFileViewDelegate.modelData.color
            textFont: SettingsModel.logLinesFont
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
            fullFileView.autoScrollEnabled = false;
            fileReaderModel.updateFilteredItemSelection(item.index, exclusive, true);
            fileReaderModel.updateItemSelection(item.lineIndex, true, true);
            fullFileView.view.positionViewAtIndex(item.lineIndex, ListView.Center);
        }

        Connections {
            target: fileReaderModel
            enabled: filteredFileView.autoScrollEnabled
            function onItemsAdded() {
                if (filteredFileView.autoScrollEnabled) {
                    filteredFileView.view.positionViewAtEnd();
                }
            }
        }
    }

    Connections {
        target: filteredFileView.toolbar
        function onProcessFilter(filterText) {
            root.processFilter(filterText);
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

    Component.onCompleted: filteredFileView.toolbar.setInitialFilter(fileReaderModel.filter)
}
