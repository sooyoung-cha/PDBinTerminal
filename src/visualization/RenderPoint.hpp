#pragma once
#include <limits>

struct RenderPoint {
    int   x = 0;
    int   y = 0;
    float depth = std::numeric_limits<float>::infinity();  
    char  pixel = ' ';                                 
    int   color_id = 0; 
    char  chainID = 0;
    char  structure = 0;
};
