qt_add_library(Ui_Settings STATIC)
qt_add_qml_module(Ui_Settings
    URI Ui.Settings
    VERSION 1.0
    QML_FILES
        SettingsWindow.qml
        SettingsMainView.qml
        ColoringPatternsSetting.qml
        FontSetting.qml
)
