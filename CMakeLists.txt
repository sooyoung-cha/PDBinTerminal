cmake_minimum_required(VERSION 3.15)

project(TerminalPDB C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

add_subdirectory(src)
add_executable(TerminalPDB main.cpp)
target_link_libraries(TerminalPDB structure visualization)

# foreach(LIBRARY ${LIBRARIES})
#   set(LIB_OBJS ${LIB_OBJS} $<TARGET_OBJECTS:${LIBRARY}>)
# endforeach()

# add_library(${PROJECT_NAME} STATIC ${LIB_OBJS})