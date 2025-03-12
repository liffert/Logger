pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Utility

Item {
    id: root

    required property int index

    readonly property int contentPrefferedWidth: Math.max(ListView.view.width, lineIndexItemBackground.width + textItem.width + textItem.anchors.leftMargin)

    property int lineIndex: root.index
    property string text: ""
    property bool isSelected: false
    property color textColor: Style.regularTextColor
    property font textFont: font
    property int indexLineWidth: Style.indexLineWidth

    width: ListView.view.contentWidth
    height: Style.logLineHeight

    onContentPrefferedWidthChanged: {
        if (ListView.view.contentWidth < root.contentPrefferedWidth) {
            ListView.view.contentWidth = root.contentPrefferedWidth;
        }
    }

    Rectangle {
        id: lineIndexItemBackground
        anchors.left: root.left
        anchors.top: root.top
        anchors.bottom: root.bottom
        width: root.indexLineWidth + (Style.horizontalMargin * 2)
        color: Style.backgroundColor
    }

    Rectangle {
        id: hightlight
        anchors.top: root.top
        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: root.right
        color: Style.highlightColor
        visible: false
        opacity: 0.5
    }

    Text {
        id: lineIndexItemText
        text: root.lineIndex
        textFormat: Text.PlainText
        anchors.right: lineIndexItemBackground.right
        anchors.left: lineIndexItemBackground.left
        anchors.rightMargin: Style.horizontalMargin
        anchors.verticalCenter: root.verticalCenter
        horizontalAlignment: Text.AlignRight
        color: Style.brightTextColor
        font: root.textFont
    }

    //TextInput can render instantly text which regular Text item renders within 800ms+
    TextInput {
        id: textItem
        anchors.verticalCenter: root.verticalCenter
        anchors.left: lineIndexItemBackground.right
        anchors.leftMargin: Style.horizontalMargin
        text: root.text
        readOnly: true
        color: root.textColor
        font: root.textFont
    }

    states: [
        State {
            name: "selected"
            when: root.isSelected
            PropertyChanges {
                hightlight.visible: true
                textItem.color: Style.brightTextColor
            }
        }
    ]
}
