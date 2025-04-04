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
import Qt.labs.animation
import Utility

Item {
    id: root

    readonly property alias view: listView

    property alias model: listView.model
    property alias toolbar: toolbar
    property alias autoScrollEnabled: toolbar.autoScrollEnabled

    property Component delegateComponent: null
    property int indexLineWidth: Style.indexLineWidth

    signal updateItemsSelection(var startIndex, var endIndex, var value)
    signal itemSelected(var item, var exclusive)
    signal itemDoubleClicked(var item, var exclusive)

    TextViewToolbar {
        id: toolbar
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top

        onAutoScrollEnabledChanged: {
            if (toolbar.autoScrollEnabled) {
                listView.positionViewAtEnd();
            }
        }
    }

    Rectangle {
        id: background
        anchors.fill: listView
        color: Style.textBackgroundColor
    }

    ListViewEnhanced {
        id: listView
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: toolbar.bottom
        anchors.bottom: root.bottom
        delegate: root.delegateComponent

        onHeightChanged: {
            if (toolbar.autoScrollEnabled) {
                listView.positionViewAtEnd();
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: listView

        readonly property int defaultX: mouseArea.width / 2

        property int firstMultiselectionIndex: -1
        property int lastMultiselectionIndex: -1

        function findItemUnderMouse() {
            return listView.itemAt(mouseArea.defaultX, mouseArea.mouseY + listView.contentY);
        }

        function checkMultiselection() {
            if (mouseArea.lastMultiselectionIndex !== -1) {
                const itemUnderMouse = mouseArea.findItemUnderMouse();
                if (Boolean(itemUnderMouse)) {
                    const itemUnderMouseIndex = itemUnderMouse.index;
                    if (mouseArea.lastMultiselectionIndex !== itemUnderMouseIndex) {
                        const isCurrentDirectionDown = itemUnderMouseIndex > mouseArea.lastMultiselectionIndex;
                        if (mouseArea.lastMultiselectionIndex !== mouseArea.firstMultiselectionIndex) {
                            const isInitialDirectionDown = (mouseArea.firstMultiselectionIndex - mouseArea.lastMultiselectionIndex) < 0;

                            if (isInitialDirectionDown) {
                                if (!isCurrentDirectionDown) {
                                    const endIndex = Math.max(mouseArea.firstMultiselectionIndex, itemUnderMouseIndex) + 1;
                                    root.updateItemsSelection(mouseArea.lastMultiselectionIndex, endIndex, false);
                                    mouseArea.lastMultiselectionIndex = endIndex;
                                }
                            } else if (isCurrentDirectionDown) {
                                const endIndex = Math.min(mouseArea.firstMultiselectionIndex, itemUnderMouseIndex) - 1;
                                root.updateItemsSelection(mouseArea.lastMultiselectionIndex, endIndex, false);
                                mouseArea.lastMultiselectionIndex = endIndex;
                            }
                        }

                        const startIndex = mouseArea.lastMultiselectionIndex >= 0 ? (mouseArea.lastMultiselectionIndex + (isCurrentDirectionDown ? 1 : -1))
                                                                                  : itemUnderMouseIndex;

                        root.updateItemsSelection(startIndex, itemUnderMouseIndex, true);
                        mouseArea.lastMultiselectionIndex = itemUnderMouseIndex;
                    }
                }
            }
        }

        onPressed: function(mouse) {
            const itemUnderMouse = mouseArea.findItemUnderMouse();
            if (Boolean(itemUnderMouse)) {
                toolbar.autoScrollEnabled = false;
                root.itemSelected(itemUnderMouse, mouse.modifiers ^ Qt.ControlModifier);
                mouseArea.lastMultiselectionIndex = itemUnderMouse.index;
                mouseArea.firstMultiselectionIndex = mouseArea.lastMultiselectionIndex;
            }
        }

        onDoubleClicked: function(mouse) {
            const itemUnderMouse = mouseArea.findItemUnderMouse();
            if (Boolean(itemUnderMouse)) {
                root.itemDoubleClicked(itemUnderMouse, mouse.modifiers ^ Qt.ControlModifier);
            }
        }

        onReleased: {
            mouseArea.lastMultiselectionIndex = -1;
            mouseArea.firstMultiselectionIndex = mouseArea.lastMultiselectionIndex;
        }

        onMouseYChanged: mouseArea.checkMultiselection()

        Connections {
            target: listView
            enabled: mouseArea.lastMultiselectionIndex !== -1
            function onContentYChanged() {
                if (mouseArea.lastMultiselectionIndex !== -1) {
                    Qt.callLater(mouseArea.checkMultiselection);//TODO: Check if fine for performance to update that often
                }
            }
        }
    }
}
