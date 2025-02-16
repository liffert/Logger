pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    property alias model: listView.model
    readonly property alias view: listView
    property Component delegateComponent: null

    signal copySelection()

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    contentHeight: listView.contentHeight
    clip: true

    ListView {
        id: listView
        delegate: root.delegateComponent
    }
}
