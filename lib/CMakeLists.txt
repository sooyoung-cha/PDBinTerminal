file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lodepng/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd/*.cpp"
)
file(GLOB_RECURSE LIB_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/lodepng/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/simd/*.hpp"
)

add_library(structty_utils STATIC
    ${LIB_SOURCES}
    ${LIB_HEADERS}
)

set_target_properties(structty_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(structty_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lodepng
        ${CMAKE_CURRENT_SOURCE_DIR}/simd
)

if (MSVC)
    target_compile_options(structty_utils PRIVATE /W4)
else()
    target_compile_options(structty_utils PRIVATE -Wall -Wextra -Wpedantic)
endif()
