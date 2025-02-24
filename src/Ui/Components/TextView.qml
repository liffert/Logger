pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Qt.labs.animation

Item {
    id: root

    readonly property alias view: listView

    property alias model: listView.model
    property alias effectiveScrollBarWidth: scrollView.effectiveScrollBarWidth

    property Component delegateComponent: null

    signal updateItemsSelection(var startIndex, var endIndex, var value)
    signal itemSelected(var item, var exclusive)
    signal itemDoubleClicked(var item, var exclusive)

    ScrollView {
        id: scrollView
        anchors.fill: root

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        contentHeight: listView.contentHeight
        contentWidth: listView.contentWidth

        ListView {
            id: listView
            delegate: root.delegateComponent
            clip: true
            flickableDirection: Flickable.AutoFlickDirection

            //Make scrolling faster.
            //TODO: Add settings for each scrolling speed.
            WheelHandler {
                target: listView
                property: "contentY"
                rotationScale: -5
                orientation: Qt.Vertical
            }

            WheelHandler {
                target: listView
                property: "contentX"
                rotationScale: -5
                orientation: Qt.Horizontal
            }

            BoundaryRule on contentY {
                minimum: listView.originY
                maximum: listView.originY + listView.contentHeight - listView.height
                returnDuration: 0
            }

            BoundaryRule on contentX {
                minimum: listView.originX
                maximum: listView.originX + listView.contentWidth - listView.width
                returnDuration: 0
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: scrollView
        anchors.rightMargin: scrollView.effectiveScrollBarWidth
        anchors.bottomMargin: scrollView.effectiveScrollBarHeight

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
