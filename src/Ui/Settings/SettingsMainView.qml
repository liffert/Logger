pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Models

Item {
    id: root

    Button {
        text: "fontLine"
        onClicked: fontDialog2.open()
        anchors.centerIn: root
    }

    FontDialog {
        id: fontDialog2

        selectedFont: SettingsModel.logLinesFont

        onAccepted: {
            console.log("MYLOG ", fontDialog2.selectedFont);
            SettingsModel.updateLogLinesFont(fontDialog2.selectedFont);
        }
    }

}
