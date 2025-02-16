pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    required property int index

    property int lineIndex: root.index
    property string text: ""
    property bool selected: false

    signal selected(var exclusive)
    signal doubleClicked(var exclusive)

    width: parent.width
    height: textItem.height + 10

    Text {
        id: textItem
        anchors.verticalCenter: root.verticalCenter
        anchors.left: root.left
        anchors.leftMargin: 10
        text: root.text
    }

    MouseArea {
        id: mouseArea
        anchors.fill: root
        propagateComposedEvents: true
        onPressed: function(mouse) {
            root.selected(mouse.modifiers ^ Qt.ControlModifier);
        }
        onDoubleClicked: function(mouse) {
            root.selected(mouse.modifiers ^ Qt.ControlModifier);
            root.doubleClicked(mouse.modifiers ^ Qt.ControlModifier);
        }
    }

    states: [
        State {
            name: "selected"
            when: root.selected
            PropertyChanges {
                root.color: "blue"
                textItem.color: "white"
            }
        }
    ]
}
