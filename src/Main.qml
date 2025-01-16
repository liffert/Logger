import QtQuick
import QtQuick.Controls
import Ui.MainView

ApplicationWindow {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("Logger Enhanced")

    MainView {
        id: mainView
        width: root.width
        height: root.height
    }
}
