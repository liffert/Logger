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

    OpenedFilesTabBar {
        id: tabBar
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top
    }

    StackLayout {
        id: openedFiles

        anchors.left: root.left
        anchors.right: root.right
        anchors.top: tabBar.bottom
        anchors.bottom: root.bottom
        anchors.topMargin: 20

        currentIndex: tabBar.currentIndex

        Repeater {
            id: openedFilesViews
            model: FileSystemWatcher.openedFilesModel
            delegate: OpenedFileView {
                id: openedFileView

                required property var modelData

                filePath: modelData.path

                Connections {
                    target: copyShortcut
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.copy(false);
                        }
                    }
                }

                Connections {
                    target: copyAllShortcut
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.copy(true);
                        }
                    }
                }

                Connections {
                    target: deselectShortcut
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.deselect();
                        }
                    }
                }

                Connections {
                    target: selectAllShortcut
                    function onActivated() {
                        if (openedFileView.StackLayout.isCurrentItem) {
                            openedFileView.selectAll();
                        }
                    }
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
}
