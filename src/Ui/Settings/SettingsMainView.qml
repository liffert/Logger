pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Ui.Components
import Models
import Utility

Item {
    id: root

    FontSetting {
        id: fontSetting
        anchors.top: root.top
        anchors.left: root.left
        anchors.right: root.right
    }

    Rectangle {
        id: colorPatternsText
        border.width: Style.borderWidth
        border.color: Style.backgroundColor
        anchors.top: fontSetting.bottom
        anchors.left: root.left
        anchors.right: root.right
        anchors.topMargin: -Style.borderWidth
        height: Math.max(fontSetting.height, colorPatternsTextItem.height + (Style.verticalMargin * 2))

        Text {
            id: colorPatternsTextItem
            anchors.left: colorPatternsText.left
            anchors.right: colorPatternsText.right
            anchors.verticalCenter: colorPatternsText.verticalCenter
            anchors.leftMargin: Style.horizontalMargin
            anchors.topMargin: Style.horizontalMargin
            text: "Color patterns"
        }
    }

    ColoringPatternsSetting {
        id: coloringPatterns
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: colorPatternsText.bottom
        anchors.topMargin: -Style.borderWidth
        anchors.bottom: root.bottom
    }
}
