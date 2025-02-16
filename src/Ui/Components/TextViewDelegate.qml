pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    required property int index

    property int lineIndex: root.index
    property string text: ""
    property bool isSelected: false

    width: parent?.width ?? root.width//TO CHECK ON BINDING LOOP
    height: textItem.height + 10

    Text {
        id: textItem
        anchors.verticalCenter: root.verticalCenter
        anchors.left: root.left
        anchors.leftMargin: 10
        text: root.text
    }

    states: [
        State {
            name: "selected"
            when: root.isSelected
            PropertyChanges {
                root.color: "blue"
                textItem.color: "white"
            }
        }
    ]
}
