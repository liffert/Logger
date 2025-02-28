import QtQuick
import QtQuick.Controls
import Ui.MainView
import Ui.Settings

ApplicationWindow {
    id: root
    width: 1800
    height: 900
    visible: true
    title: qsTr("Logger Enhanced")

    header: ToolBar {
        ToolButton {
            text: "Settings"
            onClicked: settingsWindow.show()
        }
    }

    MainView {
        id: mainView
        width: root.width
        height: root.height
    }

    SettingsWindow {
        id: settingsWindow
    }

    //TODO set minimum height and width
}
