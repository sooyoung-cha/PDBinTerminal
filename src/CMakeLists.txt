include_directories(structure)
add_subdirectory(structure)
add_subdirectory(visualization)

find_package(OpenMP QUIET)
if (OPENMP_FOUND)
elseif (REQUIRE_OPENMP)
    message(FATAL_ERROR "-- Could not find OpenMP. Skip check with -DREQUIRE_OPENMP=0.")
endif ()
