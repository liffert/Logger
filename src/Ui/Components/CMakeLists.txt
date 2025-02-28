qt_add_library(Ui_Components STATIC)
qt_add_qml_module(Ui_Components
    URI Ui.Components
    VERSION 1.0
    QML_FILES
        TextView.qml
        TextViewDelegate.qml
        TextViewToolbar.qml
        ListViewEnhanced.qml
)
