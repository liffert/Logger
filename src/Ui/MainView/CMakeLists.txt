qt_add_library(Ui_MainView STATIC)
qt_add_qml_module(Ui_MainView
    URI Ui.MainView
    VERSION 1.0
    QML_FILES
        MainView.qml
        NoFileOpenedView.qml
)
