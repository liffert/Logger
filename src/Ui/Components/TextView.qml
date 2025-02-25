pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Qt.labs.animation

Item {
    id: root

    readonly property alias view: listView

    property alias model: listView.model

    property Component delegateComponent: null

    signal updateItemsSelection(var startIndex, var endIndex, var value)
    signal itemSelected(var item, var exclusive)
    signal itemDoubleClicked(var item, var exclusive)

    Rectangle {
        anchors.fill: parent
        color: "grey"
    }

    ListView {
        id: listView
        anchors.fill: root
        anchors.rightMargin: listView.ScrollBar.vertical.width
        anchors.bottomMargin: listView.ScrollBar.horizontal.height
        clip: true
        flickableDirection: Flickable.AutoFlickDirection
        delegate: root.delegateComponent

        //Make scrolling faster.
        //TODO: Add settings for each scrolling speed.
        WheelHandler {
            target: listView
            property: "contentY"
            rotationScale: -1
            orientation: Qt.Vertical
        }

        WheelHandler {
            target: listView
            property: "contentX"
            rotationScale: -1
            orientation: Qt.Horizontal
        }

        BoundaryRule on contentY {
            minimum: listView.originY
            maximum: listView.originY + Math.max(listView.height, listView.contentHeight) - listView.height
            returnDuration: 0
        }

        BoundaryRule on contentX {
            minimum: listView.originX
            maximum: listView.originX + Math.max(listView.width, listView.contentWidth) - listView.width
            returnDuration: 0
        }

        ScrollBar.vertical: ScrollBar {
            parent: listView.parent
            anchors.top: listView.top
            anchors.bottom: listView.bottom
            anchors.left: listView.right
            policy: ScrollBar.AlwaysOn
        }

        ScrollBar.horizontal: ScrollBar {
            parent: listView.parent
            anchors.top: listView.bottom
            anchors.left: listView.left
            anchors.right: listView.right
            policy: ScrollBar.AlwaysOn
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: listView
        anchors.rightMargin: listView.ScrollBar.vertical.width
        anchors.bottomMargin: listView.ScrollBar.horizontal.height

        readonly property int defaultX: mouseArea.width / 2

        property int firstMultiselectionIndex: -1
        property int lastMultiselectionIndex: -1
        property int listViewPosition: listView.contentY - listView.originY

        function findItemUnderMouse() {
            return listView.itemAt(mouseArea.defaultX, mouseArea.mouseY + mouseArea.listViewPosition);
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
            target: mouseArea
            enabled: mouseArea.lastMultiselectionIndex !== -1
            function onListViewPositionChanged() {
                if (mouseArea.lastMultiselectionIndex !== -1) {
                    Qt.callLater(mouseArea.checkMultiselection);//TODO: Check if fine for performance to update that often
                }
            }
        }
    }
}
