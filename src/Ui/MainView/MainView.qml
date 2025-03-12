pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtCore
import Models
import Utility
import Ui.Components

Item {
    id: root

    function openNewFile() {
        tabBar.openNewFile();
    }

    Rectangle {
        id: background
        anchors.fill: openedFiles
        color: Style.backgroundColor
    }

    OpenedFilesTabBar {
        id: tabBar
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top
        openedFilesModel: openedFilesModel
    }

    StackLayout {
        id: openedFiles

        anchors.left: root.left
        anchors.right: root.right
        anchors.top: tabBar.bottom
        anchors.bottom: root.bottom

        currentIndex: tabBar.currentIndex

        Repeater {
            id: openedFilesViews
            model: openedFilesModel.model
            delegate: OpenedFileView {
                id: openedFileView

                required property var modelData
                required property int index

                filePath: openedFileView.modelData.path
                filterText: openedFileView.modelData.filter

                onProcessFilter: function(filterText) {
                    openedFilesModel.updateFilter(openedFileView.index, filterText);
                }

                Connections {
                    id: fullFileViewAutoScrollEnabledConnection
                    target: openedFileView
                    enabled: false
                    function onFullFileViewAutoScrollEnabledChanged() {
                        openedFilesModel.updateFullFileViewAutoScroll(openedFileView.index, openedFileView.fullFileViewAutoScrollEnabled);
                    }
                }

                Connections {
                    id: filteredFileViewAutoScrollEnabledConnection
                    target: openedFileView
                    enabled: false
                    function onFilteredFileViewAutoScrollEnabledChanged() {
                        openedFilesModel.updateFilteredFileViewAutoScroll(openedFileView.index, openedFileView.filteredFileViewAutoScrollEnabled);
                    }
                }

                Connections {
                    target: copyShortcut
                    enabled: openedFileView.StackLayout.isCurrentItem
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.copy(false);
                        }
                    }
                }

                Connections {
                    target: copyAllShortcut
                    enabled: openedFileView.StackLayout.isCurrentItem
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.copy(true);
                        }
                    }
                }

                Connections {
                    target: deselectShortcut
                    enabled: openedFileView.StackLayout.isCurrentItem
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.deselect();
                        }
                    }
                }

                Connections {
                    target: selectAllShortcut
                    enabled: openedFileView.StackLayout.isCurrentItem
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.selectAll();
                        }
                    }
                }

                Connections {
                    target: autoScroll
                    enabled: openedFileView.StackLayout.isCurrentItem
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.toggleAutoScroll();
                        }
                    }
                }

                Component.onCompleted: {
                    openedFileView.fullFileViewAutoScrollEnabled = openedFileView.modelData.fullFileViewAutoScrollEnabled;
                    openedFileView.filteredFileViewAutoScrollEnabled = modelData.filteredFileViewAutoScrollEnabled;
                    fullFileViewAutoScrollEnabledConnection.enabled = true;
                    filteredFileViewAutoScrollEnabledConnection.enabled = true;
                }
            }
        }
    }

    Loader {
        anchors.fill: root
        active: openedFiles.count === 0
        sourceComponent: NoFileOpenedView {
            id: noFileOpened
            anchors.fill: parent
        }
    }

    Shortcut {
        id: copyShortcut
        context: Qt.ApplicationShortcut
        sequences: [StandardKey.Copy]
    }

    Shortcut {
        id: selectAllShortcut
        context: Qt.ApplicationShortcut
        sequences: [StandardKey.SelectAll]
    }

    Shortcut {
        id: openFileShortcut
        context: Qt.ApplicationShortcut
        sequences: [StandardKey.Open]
        onActivated: tabBar.openNewFile()
    }

    Shortcut {
        id: copyAllShortcut
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Shift+C"
    }

    Shortcut {
        id: deselectShortcut
        context: Qt.ApplicationShortcut
        sequence: "Esc"
    }

    Shortcut {
        id: autoScroll
        context: Qt.ApplicationShortcut
        sequence: "F"
    }

    OpenedFilesModel {
        id: openedFilesModel
    }
}
