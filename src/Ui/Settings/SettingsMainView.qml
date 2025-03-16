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

                ToolTip.visible: coloringStrategySelector.hovered
                ToolTip.text: "\"On read\": color assigned to each line during reading from the file. Gives smoother vertical scrolling experience than \"on render\" strategy due to \"color once\" approach outside of GUI thread. Change in coloring patterns lead to re\-reading while file.\n\n\"On render\": color assigned when line is being rendered. Supports change in coloring patterns without re-reading the file. Loads huge files much faster than \"on read\" appoach because coloring is skiped for not visible items in the view. Can lead to choppier vertical scrolling experience due to coloring on rendering.\n\nRecommended setting is \"On read\". Choose \"On render\" in case if speed of reading files is not satisfying or updating of coloring patterns without re-reading the whole file is a must, but be aware that using this strategy can affect UI smoothness."

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
