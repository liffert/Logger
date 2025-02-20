import QtQuick

Item {
    id: root

    Text {
        id: statusText
        anchors.centerIn: parent
        text: "Looks like you have no file opened\nIn order to open file please press on \"open file\" button in the top left corner"
    }
}
