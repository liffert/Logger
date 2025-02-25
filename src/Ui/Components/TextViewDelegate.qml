pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Item {
    id: root

    required property int index

    readonly property int contentPrefferedWidth: Math.max(ListView.view.width, root.childrenRect.width)

    //TODO: Maybe think of something better
    property int lineIndex: root.index
    property string text: ""
    property bool isSelected: false
    property color textColor: Qt.color("black")

    width: ListView.view.contentWidth
    height: Math.max(textItem.height, 20)

    onContentPrefferedWidthChanged: {
        if (ListView.view.contentWidth < root.contentPrefferedWidth) {
            ListView.view.contentWidth = root.contentPrefferedWidth;
        }
    }

    //TODO: Design check
    Rectangle {
        id: background
        anchors.fill: root
        color: "white"
    }

    Rectangle {
        id: lineIndexItemBackground
        anchors.left: root.left
        anchors.top: root.top
        anchors.bottom: root.bottom
        width: 100//TODO: ListView.view.count
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
        textFormat: Text.PlainText
        anchors.right: lineIndexItemBackground.right
        anchors.left: lineIndexItemBackground.left
        anchors.rightMargin: 10
        anchors.verticalCenter: root.verticalCenter
        horizontalAlignment: Text.AlignRight
        color: "white"
    }

    //TextInput can render instantly text which regular Text item renders within 800ms+
    TextInput {
        id: textItem
        anchors.verticalCenter: root.verticalCenter
        anchors.left: lineIndexItemBackground.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        text: root.text
        readOnly: true
        color: root.textColor
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
