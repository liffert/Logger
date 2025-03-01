pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Ui.Components
import Models

Item {
    id: root
    height: contnetRow.height

    Row {
        id: contnetRow
        spacing: 5
        Text {
            id: currentFont
            text: "Current font: "
            anchors.verticalCenter: selectFontButton.verticalCenter
        }

        Button {
            id: selectFontButton
            text: SettingsModel.formatFont(SettingsModel.logLinesFont)
            onClicked: fontDialog.open()
        }

        Button {
            id: resetToDefault
            text: "Reset to default"
            onClicked: SettingsModel.resetLogLinesFont()
        }
    }

    FontDialog {
        id: fontDialog

        selectedFont: SettingsModel.logLinesFont
        onAccepted: SettingsModel.updateLogLinesFont(fontDialog.selectedFont)
    }
}
