cmake_minimum_required(VERSION 3.15)
project(
    TerminalPDB #recommendation for name? 
    VERSION 1.0.0 
    DESCRIPTION "Visualization of protein structures in terminal" 
    LANGUAGES CXX #
)

add_subdirectory(src)
set(structure visualization)

foreach(LIBRARY ${LIBRARIES})
  set(LIB_OBJS ${LIB_OBJS} $<TARGET_OBJECTS:${LIBRARY}>)
endforeach()

add_library(${PROJECT_NAME} STATIC ${LIB_OBJS})