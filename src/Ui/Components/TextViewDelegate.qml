/*
 * Logger - simple log viewer application.
 * Copyright (C) 2025 Dmytro Martyniuk <digitizenetwork@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Utility

Item {
    id: root

    required property int index

    readonly property int contentPrefferedWidth: Math.max(ListView.view.width, root.indexLineWidth + textItem.width + textItem.anchors.leftMargin)

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
        width: root.indexLineWidth
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
        anchors.left: lineIndexItemBackground.left
        anchors.right: lineIndexItemBackground.right
        anchors.leftMargin: Style.horizontalMargin
        anchors.rightMargin: Style.horizontalMargin
        anchors.verticalCenter: lineIndexItemBackground.verticalCenter
        text: root.lineIndex
        textFormat: Text.PlainText
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
