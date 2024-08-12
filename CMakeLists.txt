cmake_minimum_required(VERSION 3.15)
project(
    TerminalPDB #recommendation for name? 
    VERSION 1.0.0 
    DESCRIPTION "Visualization of protein structures in terminal" 
    LANGUAGES CXX #
)

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(src)