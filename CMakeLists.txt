cmake_minimum_required(VERSION 3.15)

project(StrucTTY LANGUAGES C CXX)

# Build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

add_subdirectory(lib)
add_subdirectory(src)

add_executable(StrucTTY main.cpp)

target_link_libraries(StrucTTY
    PRIVATE
        structty_core
        structty_utils
        ${CURSES_LIBRARIES}
)

if (MSVC)
    target_compile_options(StrucTTY PRIVATE /W4)
else()
    target_compile_options(StrucTTY PRIVATE -Wall -Wextra -Wpedantic)
endif()
