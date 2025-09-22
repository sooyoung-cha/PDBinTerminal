#pragma once
#include "../structure/Protein.hpp"
#include "../structure/Atom.hpp"
#include <vector>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <ncurses.h>

struct RGBA { uint8_t r,g,b,a; };

class Camera {
public:
    Camera(const std::string dir, const int width, const height);
    ~Camera();

    void screenshot(const std::vector<RenderPoint>& screenPixels);    
    PngImage renderPoint2png(const std::vector<RenderPoint>& screenPixels);
    void save_png(PngImage target);

private:
    std::string camera_dir;
    int camera_width, camera_height;

};