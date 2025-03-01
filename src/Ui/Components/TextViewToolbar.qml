pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    color: "white"

    property alias autoScrollEnabled: autoScrollOption.checked
    property bool showFilter: false

    function setInitialFilter(filterText) {
        filter.text = filterText;
    }

    signal processFilter(var filterText)

    height: filterText.implicitHeight + 10

    //TODO_LOW: activity indicator
    Item {
        id: filterItem
        anchors.top: root.top
        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: autoScrollOption.left
        anchors.rightMargin: 10
        visible: root.showFilter

        Text {
            id: filterText
            text: "CurrentFilter: "
            anchors.left: filterItem.left
            anchors.top: filterItem.top
            anchors.bottom: filterItem.bottom
        }

        //TODO_LOW: Make history of previous searches
        TextInput {
            id: filter
            anchors.left: filterText.right
            anchors.right: filterItem.right
            anchors.top: filterItem.top
            anchors.bottom: filterItem.bottom
            clip: true

            onAccepted: root.processFilter(filter.displayText)
        }
    }

    CheckBox {
        id: autoScrollOption
        anchors.right: root.right
        text: "Auto Scroll"
        checked: true
    }
}
