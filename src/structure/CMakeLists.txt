# project(structure)
# file(GLOB SOURCES *.cpp)
# add_library(${PROJECT_NAME} OBJECT ${SOURCES})
# target_include_directories(${PROJECT_NAME} public ${CMAKE_SOURCE_DIR}/src)

set(structure_source_files
    Atom.hpp
    Parameters.cpp
    Parameters.hpp
    Protein.cpp
    Protein.hpp
    StructureMaker.cpp
    StructureMaker.hpp
    SSPredictor.cpp
    SSPredictor.hpp
)

add_library(structure ${structure_source_files})