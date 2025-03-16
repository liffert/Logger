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
        id: colorPatternsHeader
        border.width: Style.borderWidth
        border.color: Style.backgroundColor
        anchors.top: fontSetting.bottom
        anchors.left: root.left
        anchors.right: root.right
        anchors.topMargin: -Style.borderWidth
        height: Math.max(fontSetting.height, colorPatternsTextItem.height + (Style.verticalMargin * 2))

        Text {
            id: colorPatternsTextItem
            anchors.left: colorPatternsHeader.left
            anchors.right: colorPatternsHeader.right
            anchors.verticalCenter: colorPatternsHeader.verticalCenter
            anchors.leftMargin: Style.horizontalMargin
            anchors.topMargin: Style.horizontalMargin
            text: "Color patterns"
        }

        Item {
            id: coloringStrategy
            anchors.right: colorPatternsHeader.right
            anchors.top: colorPatternsHeader.top
            anchors.bottom: colorPatternsHeader.bottom

            Text {
                anchors.right: coloringStrategySelector.left
                anchors.rightMargin: Style.horizontalMargin
                anchors.verticalCenter: coloringStrategy.verticalCenter
                text: "Coloring strategy:"
            }

            ComboBox {
                id: coloringStrategySelector
                anchors.top: coloringStrategy.top
                anchors.bottom: coloringStrategy.bottom
                anchors.right: coloringStrategy.right
                anchors.rightMargin: Style.horizontalMargin
                anchors.topMargin: Style.verticalMargin
                anchors.bottomMargin: Style.verticalMargin
                model: ["On read", "On render"]
                currentIndex: SettingsModel.coloringStrategy === Settings.ColoringStrategy.ON_RENDER ? 1 : 0

                onActivated: function(index) {
                    SettingsModel.coloringStrategy = index === 1 ? Settings.ColoringStrategy.ON_RENDER : Settings.ColoringStrategy.ON_READ;
                }
            }
        }
    }

    ColoringPatternsSetting {
        id: coloringPatterns
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: colorPatternsHeader.bottom
        anchors.topMargin: -Style.borderWidth
        anchors.bottom: root.bottom
    }
}
