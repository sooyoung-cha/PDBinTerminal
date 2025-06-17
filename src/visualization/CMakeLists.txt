# project(visualization)
# file(GLOB SOURCES *.cpp)
# add_library(${PROJECT_NAME} OBJECT ${SOURCES})
# target_include_directories(${PROJECT_NAME} public ${CMAKE_SOURCE_DIR}/src)

set(
    visualization_source_files
    Screen.cpp
    Screen.hpp
)

add_library(visualization ${visualization_source_files})

target_link_libraries(visualization
    PRIVATE
    structure     
    ${CURSES_LIBRARIES} 
)