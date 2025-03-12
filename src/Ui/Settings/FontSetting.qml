pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Ui.Components
import Models
import Utility

Rectangle {
    id: root
    height: contnetRow.height + (Style.verticalMargin * 2)
    border.width: Style.borderWidth
    border.color: Style.backgroundColor

    Row {
        id: contnetRow
        spacing: Style.horizontalMargin
        anchors.verticalCenter: root.verticalCenter
        anchors.left: root.left
        anchors.right: root.right
        anchors.leftMargin: Style.horizontalMargin
        anchors.rightMargin: Style.horizontalMargin

        Text {
            id: currentFont
            text: "Current font: "
            anchors.verticalCenter: selectFontButton.verticalCenter
        }

        Button {
            id: selectFontButton
            text: SettingsModel.formatFont(Style.logLineFont)
            onClicked: fontDialog.open()
        }

        Button {
            id: resetToDefault
            text: "Reset to default"
            onClicked: SettingsModel.resetLogLineFont()
        }
    }

    FontDialog {
        id: fontDialog

        selectedFont: Style.logLineFont
        onAccepted: SettingsModel.updateLogLineFont(fontDialog.selectedFont)
    }
}
