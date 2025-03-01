pragma ComponentBehavior: Bound

import QtQuick
import Models

Window {
    id: root
    title: "Settings"
    width: 1000
    height: 600

    SettingsMainView {
        width: root.width
        height: root.height
    }

    onVisibleChanged: {
        if (root.visible) {
            SettingsModel.openSettings();
        } else {
            SettingsModel.closeSettings()
        }
    }
}
