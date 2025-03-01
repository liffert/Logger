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

            required property var modelData
            required property int index
            property bool isHeld: false

            height: contentLayout.height
            width: ListView.view.contentWidth
            opacity: Boolean(addItem.editedDelegate) && delegate != addItem.editedDelegate ? 0.5 : 1.0

            //TODO: reset contentWidth on removing elements
            Row {
                id: contentLayout
                height: Math.max(40, patternText.height + 10)
                spacing: 10

                Drag.active: delegate.isHeld
                Drag.source: delegate
                Drag.hotSpot.x: delegate.width / 2
                Drag.hotSpot.y: delegate.height / 2

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

                CheckBox {
                    anchors.verticalCenter: contentLayout.verticalCenter

                    text: "Case Sensitive"
                    checked: delegate.modelData.caseSensitive
                }

                Text {
                    id: patternText
                    anchors.verticalCenter: contentLayout.verticalCenter
                    text: delegate.modelData.pattern
                    color: delegate.modelData.color
                }

                states: [
                    State {
                        when: delegate.isHeld
                        ParentChange {
                            target: contentLayout
                            parent: listView
                        }
                    }
                ]
            }

            DropArea {
                anchors.fill: delegate
                anchors.margins: 5
                onEntered: function(drag) {
                    SettingsModel.moveColoringPattern(drag.source.index, delegate.index)
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: delegate
                drag.target: delegate.isHeld ? contentLayout : undefined
                drag.axis: Drag.YAxis
                drag.smoothed: false
                drag.threshold: 0
                drag.maximumY: listView.height - (delegate.height / 2)
                drag.minimumY: 0 - (delegate.height / 2)

                onPressed: {
                    if (!Boolean(addItem.editedDelegate)) {
                        delegate.isHeld = true
                    }
                }
                onReleased: delegate.isHeld = false
                onDoubleClicked: {
                    if (!Boolean(addItem.editedDelegate)) {
                        addItem.editedDelegate = delegate;
                    }
                }
            }
        }
    }

    Rectangle {
        id: addItem

        property var editedDelegate: null

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
            anchors.right: addItemOptionsRow.left
            anchors.leftMargin: 5
            anchors.verticalCenter: addItem.verticalCenter
        }

        Row {
            id: addItemOptionsRow
            anchors.right: addItem.right
            anchors.top: addItem.top
            anchors.bottom: addItem.bottom
            anchors.rightMargin: 5
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            Button {
                id: colorPicker
                anchors.top: addItemOptionsRow.top
                anchors.bottom: addItemOptionsRow.bottom

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
                id: caseSensitiveOption
                anchors.verticalCenter: addItemOptionsRow.verticalCenter

                text: "Case Sensitive"
                checked: false
            }

            Button {
                id: confirmButton
                anchors.top: addItemOptionsRow.top
                anchors.bottom: addItemOptionsRow.bottom
                text: "Add pattern"
                onClicked: {
                    if (Boolean(addItem.editedDelegate)) {
                        SettingsModel.updatePattern(colorPatternInput.displayText, colorPicker.patternColor, caseSensitiveOption.checked, addItem.editedDelegate.index)
                        addItem.editedDelegate = null;
                    } else {
                        SettingsModel.addPattern(colorPatternInput.displayText, colorPicker.patternColor, caseSensitiveOption.checked);
                        colorPatternInput.clear();
                        colorPicker.patternColor = "black";
                        caseSensitiveOption.checked = false;
                    }
                }
            }

            Button {
                id: deletePattern
                anchors.top: addItemOptionsRow.top
                anchors.bottom: addItemOptionsRow.bottom
                text: "Delete Pattern"
                visible: false
                onClicked: SettingsModel.deletePattern(addItem.editedDelegate?.index ?? -1)
            }

            Button {
                id: exitEdit
                anchors.top: addItemOptionsRow.top
                anchors.bottom: addItemOptionsRow.bottom
                text: "Exit edit"
                visible: false
                onClicked: addItem.editedDelegate = null
            }
        }

        states: [
            State {
                when: Boolean(addItem.editedDelegate)
                PropertyChanges {
                    deletePattern.visible: true
                    exitEdit.visible: true
                    confirmButton.text: "Confirm edit"
                    addItemText.text: "Edit color pattern"
                    colorPicker.patternColor: addItem.editedDelegate.modelData.color
                    colorPatternInput.text: addItem.editedDelegate.modelData.pattern
                    caseSensitiveOption.checked: addItem.editedDelegate.modelData.caseSensitive
                }
            }
        ]
    }

    ColorDialog {
        id: colorDialog
        onAccepted: colorPicker.patternColor = colorDialog.selectedColor
    }
}
