pragma ComponentBehavior: Bound

import QtQuick

Window {
    id: root
    title: "Settings"

    SettingsMainView {
        width: root.width
        height: root.height
    }
}
