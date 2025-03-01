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
