file(GLOB_RECURSE STRUCT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/structure/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/visualization/*.cpp"
)
file(GLOB_RECURSE STRUCT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/structure/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/visualization/*.hpp"
)

add_library(structty_core STATIC
    ${STRUCT_SOURCES}
    ${STRUCT_HEADERS}
)

set_target_properties(structty_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(structty_core
    PUBLIC     
        ${CMAKE_CURRENT_SOURCE_DIR}/structure
        ${CMAKE_CURRENT_SOURCE_DIR}/visualization
)

target_link_libraries(structty_core
    PRIVATE
        structty_utils
)

if (MSVC)
    target_compile_options(structty_core PRIVATE /W4)
else()
    target_compile_options(structty_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
