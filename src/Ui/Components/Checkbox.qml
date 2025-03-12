pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Utility

CheckBox {
    id: root

    contentItem: Text {
        text: root.text
        color: Style.brightTextColor
        leftPadding: root.indicator.width + root.spacing
    }
}
