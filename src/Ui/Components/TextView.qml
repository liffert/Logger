pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Qt.labs.animation

Item {
    id: root

    readonly property alias view: listView

    property alias model: listView.model
    property alias toolbar: toolbar
    property alias autoScrollEnabled: toolbar.autoScrollEnabled

    property Component delegateComponent: null

    signal updateItemsSelection(var startIndex, var endIndex, var value)
    signal itemSelected(var item, var exclusive)
    signal itemDoubleClicked(var item, var exclusive)

    //TODO: design check
    Rectangle {
        anchors.fill: root
        color: "grey"
    }

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
