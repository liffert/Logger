pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Models

Item {
    id: root

    FileSelectionMenu {
        id: fileSelection
        anchors.top: root.top
        anchors.left: root.left
        anchors.right: root.right
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.topMargin: 20

        onFileAccepted: function(path) {
            fileReaderModel.openFile(path);
        }
    }

    ScrollView {
        id: scrollView
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: fileSelection.bottom
        anchors.topMargin: 20
        anchors.bottom: scrollView2.top
        anchors.bottomMargin: 50
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        contentWidth: Math.max(fileReaderModel.modelWidth + 10, scrollView.width)
        contentHeight: list.contentHeight
        clip: true

        ListView {
            id: list
            model: fileReaderModel.model

            delegate: Rectangle {
                id: delegate
                width: scrollView.contentWidth
                height: textItem.height + 10

                required property var modelData

                Text {
                    id: textItem
                    anchors.verticalCenter: delegate.verticalCenter
                    anchors.left: delegate.left
                    anchors.leftMargin: 10
                    text: delegate.modelData.text
                    //font.pixelSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        console.log("MYLOG");
                    }
                }
            }
        }
    }

    ScrollView {
        id: scrollView2
        anchors.left: root.left
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: 300
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        contentWidth: Math.max(fileReaderModel.filteredModelWidth + 10, scrollView.width)
        contentHeight: list2.contentHeight
        clip: true

        ListView {
            id: list2
            model: fileReaderModel.filteredModel

            delegate: Rectangle {
                id: delegate2
                width: scrollView2.contentWidth
                height: textItem2.height + 10

                required property var modelData

                Text {
                    id: textItem2
                    anchors.verticalCenter: delegate2.verticalCenter
                    anchors.left: delegate2.left
                    anchors.leftMargin: 10
                    text: delegate2.modelData.text
                    //font.pixelSize: 14
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        console.log("MYLOG");
                    }
                }
            }
        }
    }


    Rectangle {
        color: "white"
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: scrollView.bottom
        height: filterText.height
    }

    Text {
        id: filterText
        text: "CurrentFilter: "
        anchors.left: root.left
        anchors.top: filter.top
    }

    TextEdit {
        id: filter
        anchors.left: filterText.right
        anchors.right: root.right
        anchors.top: scrollView.bottom
        height: filterText.height
    }

    FileReaderModel {
        id: fileReaderModel
        filter: filter.text

        onModelWidthChanged: {
            console.log("Model: ", modelWidth);
        }
        onFilteredModelWidthChanged: {
            console.log(filteredModelWidth);
        }
    }
}
