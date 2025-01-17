set(UTILITY_HEADERS )
set(UTILITY_SOURCES )

add_subdirectory(Models/)

qt_add_library(Utility
    STATIC
    ${UTILITY_HEADERS}
    ${UTILITY_SOURCES}
)

target_link_libraries(Utility
    PRIVATE
    Qt6::Core
)

target_include_directories(Utility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
