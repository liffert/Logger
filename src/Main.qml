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

import QtQuick
import QtQuick.Controls
import Ui.MainView
import Ui.Settings

ApplicationWindow {
    id: root
    width: 1800
    height: 900
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    title: qsTr("Logger Enhanced")

    menuBar: MenuBar {
        Menu {
            title: "File"
            Action {
                text: "Open file"
                onTriggered: mainView.openNewFile()
            }
            Action {
                text: "Settings"
                onTriggered: settingsWindow.show()
            }
        }
    }

    MainView {
        id: mainView
        width: root.width
        height: root.height - root.menuBar.height
    }

    SettingsWindow {
        id: settingsWindow
    }
}
