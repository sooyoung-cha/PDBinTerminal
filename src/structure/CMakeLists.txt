project(structure)
file(GLOB SOURCES *.cpp)
add_library(${PROJECT_NAME} OBJECT ${SOURCES})
target_include_directories(${PROJECT_NAME} public ${CMAKE_SOURCE_DIR}/inc)