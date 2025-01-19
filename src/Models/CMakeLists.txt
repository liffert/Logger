set(MODELS_HEADERS )
set(MODELS_SOURCES )

add_subdirectory(FileReader/)

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
)

target_link_libraries(Models
    PRIVATE
    Utility
    Qt6::Gui
)
