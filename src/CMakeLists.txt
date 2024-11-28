include_directories(structure)
include_directories(visualization)

add_subdirectory(structure)
add_subdirectory(visualization)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

target_link_libraries(visualization PRIVATE ${CURSES_LIBRARIES})

find_package(OpenMP QUIET)
if (OPENMP_FOUND)
elseif (REQUIRE_OPENMP)
    message(FATAL_ERROR "-- Could not find OpenMP. Skip check with -DREQUIRE_OPENMP=0.")
endif ()
