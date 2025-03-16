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
import Qt.labs.animation

ListView {
    id: root
    anchors.rightMargin: root.ScrollBar.vertical.width
    anchors.bottomMargin: root.ScrollBar.horizontal.height
    clip: true
    flickableDirection: Flickable.AutoFlickDirection
    interactive: false

    //Custom scrolling to make scrolling faster.
    //TODO_LOW: Add settings for each scrolling speed.
    WheelHandler {
        target: root
        property: "contentY"
        rotationScale: -1
        orientation: Qt.Vertical
    }

    WheelHandler {
        target: root
        property: "contentX"
        rotationScale: -1
        orientation: Qt.Horizontal
    }

    BoundaryRule on contentY {
        minimum: root.originY
        maximum: root.originY + Math.max(root.height, root.contentHeight) - root.height
        returnDuration: 0
    }

    BoundaryRule on contentX {
        minimum: root.originX
        maximum: root.originX + Math.max(root.width, root.contentWidth) - root.width
        returnDuration: 0
    }

    ScrollBar.vertical: ScrollBar {
        parent: root.parent
        anchors.top: root.top
        anchors.bottom: root.bottom
        anchors.left: root.right
        policy: ScrollBar.AlwaysOn
    }

    ScrollBar.horizontal: ScrollBar {
        parent: root.parent
        anchors.top: root.bottom
        anchors.left: root.left
        anchors.right: root.right
        policy: ScrollBar.AlwaysOn
    }
}
