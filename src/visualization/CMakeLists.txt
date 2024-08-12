# project(visualization)
# file(GLOB SOURCES *.cpp)
# add_library(${PROJECT_NAME} OBJECT ${SOURCES})
# target_include_directories(${PROJECT_NAME} public ${CMAKE_SOURCE_DIR}/src)

set(
    visualization_source_files
    init.cpp
    init.hpp
    loop.cpp
    loop.hpp
    PDBLoader.cpp
    PDBLoader.hpp
    Screen.cpp
    Screen.hpp
)

add_library(visualization ${visualization_source_files})