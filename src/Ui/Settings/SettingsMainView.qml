pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Ui.Components
import Models

Item {
    id: root

    FontSetting {
        id: fontSetting
        anchors.top: root.top
        anchors.left: root.left
        anchors.right: root.right
        anchors.leftMargin: 5
        anchors.topMargin: 5
    }

    Text {
        id: colorPatternsText
        anchors.top: fontSetting.bottom
        anchors.left: root.left
        anchors.right: root.right
        anchors.leftMargin: 5
        anchors.topMargin: 5
        text: "Color patterns"
    }

    ColoringPatternsSetting {
        id: coloringPatterns
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: colorPatternsText.bottom
        anchors.bottom: root.bottom
        anchors.topMargin: 5
    }
}
