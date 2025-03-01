pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Ui.Components
import Models


Rectangle {
    id: root
    border.width: 2

    ListViewEnhanced {
        id: listView
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: root.top
        anchors.bottom: addItem.top
        anchors.topMargin: 5
        anchors.leftMargin: 5

        model: SettingsModel.coloringPatternsModel
        delegate: Item {
            id: delegate

            height: contentLayout.height
            width: ListView.view.contentWidth

            required property var modelData
            required property int index

            //TODO: reset contentWidth on removing elements
            Row {
                id: contentLayout
                height: Math.max(40, patternText.height + 10)
                spacing: 10

                onWidthChanged: {
                    if (delegate.ListView.view.contentWidth < contentLayout.width) {
                        delegate.ListView.view.contentWidth = contentLayout.width;
                    }
                }

                Rectangle {
                    border.width: 2
                    anchors.top: contentLayout.top
                    anchors.bottom: contentLayout.bottom
                    anchors.topMargin: 5
                    anchors.bottomMargin: 5
                    width: 50
                    color: delegate.modelData.color
                }

                Text {
                    id: patternText
                    anchors.verticalCenter: contentLayout.verticalCenter
                    text: delegate.modelData.pattern
                    color: delegate.modelData.color
                }
            }
        }
    }

    Rectangle {
        id: addItem

        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: root.right
        height: addItemText.height + 20
        border.width: 2

        Text {
            id: addItemText
            anchors.verticalCenter: addItem.verticalCenter
            anchors.left: addItem.left
            anchors.leftMargin: 5
            text: "Input new color pattern:"
        }

        TextInput {
            id: colorPatternInput
            clip: true
            anchors.left: addItemText.right
            anchors.right: colorPicker.left
            anchors.leftMargin: 5
            anchors.verticalCenter: addItem.verticalCenter
        }

        Button {
            id: colorPicker
            anchors.right: checkSensitiveOption.left
            anchors.top: addItem.top
            anchors.bottom: addItem.bottom
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            property color patternColor: "black"

            width: 50
            background: Rectangle {
                border.width: 2
                color: colorPicker.patternColor
            }

            onClicked: {
                colorDialog.selectedColor = colorPicker.patternColor;
                colorDialog.open();
            }
        }

        CheckBox {
            id: checkSensitiveOption
            anchors.right: confirmButton.left
            anchors.top: addItem.top
            anchors.bottom: addItem.bottom
            anchors.verticalCenter: addItem.verticalCenter

            text: "Case Sensitive"
            checked: false
        }

        Button {
            id: confirmButton
            anchors.right: addItem.right
            anchors.top: addItem.top
            anchors.bottom: addItem.bottom
            anchors.rightMargin: 5
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            text: "Add pattern"
            onClicked: {
                SettingsModel.addPattern(colorPatternInput.displayText, colorPicker.patternColor, checkSensitiveOption.checked);
                colorPatternInput.clear();
                colorPicker.patternColor = "black";
                checkSensitiveOption.checked = false;
            }
        }
    }

    ColorDialog {
        id: colorDialog
        onAccepted: colorPicker.patternColor = colorDialog.selectedColor
    }
}
