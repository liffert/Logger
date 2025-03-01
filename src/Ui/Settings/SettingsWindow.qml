pragma ComponentBehavior: Bound

import QtQuick
import Models

Window {
    id: root
    title: "Settings"
    minimumWidth: 800
    minimumHeight: 600

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
