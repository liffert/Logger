pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Item {
    id: root

    required property int index

    readonly property int contentWidth: root.childrenRect.width

    //TODO: Maybe think of something better
    property int lineIndex: root.index
    property string text: ""
    property bool isSelected: false
    property color textColor: Qt.color("black")

    width: parent?.width ?? root.contentWidth
    height: 20//TODO: Check some another way to be able to change fonts

    onContentWidthChanged: {
        //TODO: Not ideal as does not reset on the rewriting at the moment.
        if (ListView.view.contentWidth < root.contentWidth) {
            ListView.view.contentWidth = root.contentWidth;
        }
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
        id: cutSymbolsTextBackground
        anchors.right: cutSymbolsText.right
        anchors.left: textItem.right
        anchors.top: root.top
        anchors.bottom: root.bottom
        color: "grey"
        visible: cutSymbolsText.visible
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
        anchors.rightMargin: 10
        anchors.verticalCenter: root.verticalCenter
        color: "white"
    }

    Text {
        id: textItem
        anchors.verticalCenter: root.verticalCenter
        anchors.left: lineIndexItemBackground.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        text: root.text.length > 2000 ? root.text.substring(0, 2000) : root.text
        textFormat: Text.PlainText
        color: root.textColor
    }

    //TODO: Need to cut somethow by width, not by symbols
    Text {
        id: cutSymbolsText
        anchors.left: textItem.right
        anchors.verticalCenter: cutSymbolsTextBackground.verticalCenter
        text: " +%1 cut symbols".arg(root.text.length - 2000)
        visible: root.text.length > 2000
        color: "white"
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
