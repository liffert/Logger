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
