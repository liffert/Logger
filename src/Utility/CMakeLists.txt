set(UTILITY_HEADERS
    FileSystemWatcher.h
)

set(UTILITY_SOURCES
    FileSystemWatcher.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core)
add_subdirectory(Models/)

#TODO: rework to regular library if no QML classes intended
qt_add_library(Utility STATIC)
qt_add_qml_module(Utility
    URI Utility
    VERSION 1.0
    SOURCES
        ${UTILITY_HEADERS}
        ${UTILITY_SOURCES}
)

target_link_libraries(Utility
    PRIVATE
    Qt6::Core
)

target_include_directories(Utility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
