pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Ui.Components
import Models

Item {
    id: root

    Row {
        id: fontSetting
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

    Item {
        id: coloringPatterns
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: fontSetting.bottom
        anchors.bottom: root.bottom

        ListViewEnhanced {
            id: listView
            anchors.fill: coloringPatterns

            model: SettingsModel.coloringPatternsModel
            delegate: Item {
                id: delegate

                height: contentLayout.height
                width: ListView.view.width

                required property var modelData
                required property int index

                Row {
                    id: contentLayout
                    spacing: 5
                    Text {
                        anchors.verticalCenter: contentLayout.verticalCenter
                        text: delegate.index + 1
                    }

                    Text {
                        anchors.verticalCenter: contentLayout.verticalCenter
                        text: delegate.modelData.pattern
                    }

                    Rectangle {
                        width: 50
                        height: 30
                        color: delegate.modelData.color
                    }
                }
            }
        }
    }


    FontDialog {
        id: fontDialog

        selectedFont: SettingsModel.logLinesFont
        onAccepted: SettingsModel.updateLogLinesFont(fontDialog.selectedFont)
    }
}
