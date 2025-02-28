pragma ComponentBehavior: Bound

import QtQuick

Window {
    id: root
    title: "Settings"
    width: 600
    height: 300

    SettingsMainView {
        width: root.width
        height: root.height
    }
}
