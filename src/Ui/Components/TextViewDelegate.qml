pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Item {
    id: root

    required property int index

    readonly property int contentWidth: root.childrenRect.width

    //Maybe think of something better
    property int lineIndex: root.index
    property string text: ""
    property bool isSelected: false

    width: parent?.width ?? root.contentWidth
    height: 20//Check some another way to be able to change fonts

    onContentWidthChanged: {
        //Not ideal as does not reset on the rewriting at the moment.
        if (ListView.view.contentWidth < root.contentWidth) {
            ListView.view.contentWidth = root.contentWidth;
        }
    }

    Rectangle {
        id: lineIndexItem
        anchors.top: root.top
        anchors.bottom: root.bottom
        width: 100//ListView.view.count
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
        anchors.right: lineIndexItem.right
        anchors.rightMargin: 10
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
