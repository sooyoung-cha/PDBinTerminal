#pragma once
#include "../structure/Protein.hpp"
#include "../structure/Atom.hpp"
#include <vector>
#include <cmath>
#include <iostream>

class Screen {
public:
    Screen(const int& width, const int& height, const bool& show_structure);
    void set_protein(Protein* protein);
    void set_zoom_level(float zoom);
    bool handle_input();
    void drawScreen();

private:
    int screen_width;
    int screen_height;
    bool screen_show_structure;
    float aspect_ratio;
    float zoom_level;
    char* mScreen;
    Protein* data;

    void project();
    void clear_screen();
    void print_screen();
};