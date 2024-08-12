# project(visualization)
# file(GLOB SOURCES *.cpp)
# add_library(${PROJECT_NAME} OBJECT ${SOURCES})
# target_include_directories(${PROJECT_NAME} public ${CMAKE_SOURCE_DIR}/src)

set(
    visualization_source_files
    visualization/init.cpp
    visualization/init.hpp
    visualization/loop.cpp
    visualization/loop.hpp
    visualization/PDBLoader.cpp
    visualization/PDBLoader.hpp
    visualization/Screen.cpp
    visualization/Screen.hpp
)