set(MODELS_HEADERS )
set(MODELS_SOURCES )

find_package(Qt6 REQUIRED COMPONENTS Gui)
add_subdirectory(FileReader/)
add_subdirectory(OpenedFiles/)

qt_add_library(Models STATIC)
qt_add_qml_module(Models
    URI Models
    VERSION 1.0
    SOURCES
        ${MODELS_HEADERS}
        ${MODELS_SOURCES}
)

target_include_directories(Models
    PRIVATE
        FileReader/
        OpenedFiles/
)

target_link_libraries(Models
    PRIVATE
    Utility
    Qt6::Gui
)
