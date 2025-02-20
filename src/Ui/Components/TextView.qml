pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Item {
    id: root

    readonly property alias view: listView

    property alias model: listView.model
    property alias contentWidth: scrollView.contentWidth
    property Component delegateComponent: null

    signal copySelection()
    signal updateItemSelection(var index, var value)
    signal itemSelected(var item, var exclusive)
    signal itemDoubleClicked(var item, var exclusive)

    ScrollView {
        id: scrollView
        anchors.fill: root

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        contentHeight: listView.contentHeight
        clip: true

        ListView {
            id: listView
            delegate: root.delegateComponent
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
                        if (mouseArea.lastMultiselectionIndex !== mouseArea.firstMultiselectionIndex) {
                            const isInitialDirectionDown = (mouseArea.firstMultiselectionIndex - mouseArea.lastMultiselectionIndex) < 0;
                            const isCurrentDirectionDown = itemUnderMouseIndex > mouseArea.lastMultiselectionIndex;

                            if (isInitialDirectionDown) {
                                if (!isCurrentDirectionDown) {
                                    root.updateItemSelection(mouseArea.lastMultiselectionIndex, false);
                                }
                            } else if (isCurrentDirectionDown) {
                                root.updateItemSelection(mouseArea.lastMultiselectionIndex, false)
                            }
                        }
                        root.updateItemSelection(itemUnderMouseIndex, true);
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
                    mouseArea.checkMultiselection();
                }
            }
        }
    }

}
