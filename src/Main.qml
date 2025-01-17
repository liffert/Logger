import QtQuick
import QtQuick.Controls
import Ui.MainView

ApplicationWindow {
    id: root
    width: 1800
    height: 900
    visible: true
    title: qsTr("Logger Enhanced")

    MainView {
        id: mainView
        width: root.width
        height: root.height
    }
}
