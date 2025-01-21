cmake_minimum_required(VERSION 3.16)

project(LoggerEnhanced VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_policy(SET QTP0001 NEW)
qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appLoggerEnhanced
    Main.cpp
)

qt_add_qml_module(appLoggerEnhanced
    URI LoggerEnhanced
    VERSION 1.0
    QML_FILES
        Main.qml
)

add_subdirectory(Utility/)
add_subdirectory(Ui/MainView/)
add_subdirectory(Ui/Components/)
add_subdirectory(Models/)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appLoggerEnhanced PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appLoggerEnhanced
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appLoggerEnhanced
    PRIVATE
    Qt6::Quick
    Ui_MainViewplugin
    Ui_Componentsplugin
    Modelsplugin
    Utilityplugin
)

include(GNUInstallDirs)
install(TARGETS appLoggerEnhanced
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
