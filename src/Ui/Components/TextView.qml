pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    property alias model: listView.model
    property bool enableScrollToItemRecognition: false

    signal itemSelected(int index, bool exclusive)
    signal scrollToItem(int index)
    signal copySelection()

    function highlightItem(index) {
        root.itemSelected(index, true);
        listView.positionViewAtIndex(index, ListView.Center);
    }

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    contentHeight: listView.contentHeight
    clip: true

    ListView {
        id: listView

        delegate: Rectangle {
            id: delegate

            required property var modelData
            required property int index

            width: root.contentWidth
            height: textItem.height + 10

            Text {
                id: textItem
                anchors.verticalCenter: delegate.verticalCenter
                anchors.left: delegate.left
                anchors.leftMargin: 10
                text: delegate.modelData.text
            }

            MouseArea {
                id: mouseArea
                anchors.fill: delegate
                propagateComposedEvents: true
                onPressed: function(mouse) {
                    listView.forceActiveFocus();
                    root.itemSelected(delegate.index, mouse.modifiers ^ Qt.ControlModifier);
                    console.log("PREESSED");
                }
                onDoubleClicked: {
                    if (root.enableScrollToItemRecognition) {
                        root.scrollToItem(delegate.modelData.originalIndex);
                    }
                }
            }

            states: [
                State {
                    name: "selected"
                    when: modelData.selected
                    PropertyChanges {
                        delegate.color: "blue"
                        textItem.color: "white"
                    }
                }
            ]
        }
    }
}
