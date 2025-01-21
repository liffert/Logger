pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    property alias model: listView.model

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    contentHeight: listView.contentHeight
    clip: true

    ListView {
        id: listView

        delegate: Rectangle {
            id: delegate
            width: root.contentWidth
            height: textItem.height + 10

            required property var modelData

            Text {
                id: textItem
                anchors.verticalCenter: delegate.verticalCenter
                anchors.left: delegate.left
                anchors.leftMargin: 10
                text: delegate.modelData.text
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
