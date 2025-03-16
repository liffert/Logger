/*
 * Logger - simple log viewer application.
 * Copyright (C) 2025 Dmytro Martyniuk <digitizenetwork@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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
        visible: openedFiles.count > 0
    }

    OpenedFilesTabBar {
        id: tabBar
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top
        openedFilesModel: openedFilesModel
        visible: openedFiles.count > 0
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
