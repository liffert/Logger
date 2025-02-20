pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Item {
    id: root

    required property int index

    property int lineIndex: root.index
    property int lineIndexItemWidth: 0
    property string text: ""
    property bool isSelected: false

    width: parent?.width ?? root.width//TO CHECK ON BINDING LOOP
    height: textItem.height + 10

    Rectangle {
        id: lineIndexItem
        anchors.top: root.top
        anchors.bottom: root.bottom
        width: root.lineIndexItemWidth
        color: "grey"
    }

    Rectangle {
        id: hightlight
        anchors.top: root.top
        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: root.right
        color: "blue"
        visible: false
        opacity: 0.5
    }

    Text {
        id: lineIndexItemText
        text: root.lineIndex
        anchors.left: lineIndexItem.left
        anchors.leftMargin: 10
        anchors.verticalCenter: root.verticalCenter
        color: "white"
    }

    Text {
        id: textItem
        anchors.verticalCenter: root.verticalCenter
        anchors.left: lineIndexItem.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        text: root.text
    }

    states: [
        State {
            name: "selected"
            when: root.isSelected
            PropertyChanges {
                hightlight.visible: true
                textItem.color: "white"
            }
        }
    ]
}
