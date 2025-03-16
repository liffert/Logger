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

    property alias autoScrollEnabled: autoScrollOption.checked
    property bool showFilter: false

    function setInitialFilter(filterText) {
        filter.text = filterText;
    }

    signal processFilter(var filterText)

    height: Math.max(filterText.implicitHeight, Style.filterHeight) + (Style.verticalMargin * 2)

    //TODO_LOW: activity indicator
    Item {
        id: filterItem
        anchors.top: root.top
        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: autoScrollOption.left
        anchors.rightMargin: Style.horizontalMargin
        anchors.leftMargin: Style.horizontalMargin
        visible: root.showFilter

        Text {
            id: filterText
            text: "CurrentFilter:"
            anchors.left: filterItem.left
            anchors.verticalCenter: filterItem.verticalCenter
            color: Style.brightTextColor
        }

        Rectangle {
            id: filterBackground
            anchors.top: filterItem.top
            anchors.bottom: filterItem.bottom
            anchors.left: filterText.right
            anchors.right: filterItem.right
            anchors.leftMargin: Style.horizontalMargin
            color: Style.textBackgroundColor
            border.width: Style.borderWidth
            border.color: Style.backgroundColor
        }

        //TODO_LOW: Make history of previous searches
        TextInput {
            id: filter
            anchors.left: filterBackground.left
            anchors.right: filterBackground.right
            anchors.leftMargin: Style.horizontalMargin
            anchors.rightMargin: Style.horizontalMargin
            anchors.verticalCenter: filterItem.verticalCenter
            clip: true
            onAccepted: root.processFilter(filter.displayText)
        }
    }

    Checkbox {
        id: autoScrollOption
        anchors.right: root.right
        anchors.verticalCenter: root.verticalCenter
        text: "Auto Scroll"
        checked: true
    }
}
