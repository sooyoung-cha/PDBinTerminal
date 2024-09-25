#pragma once
#include "../structure/Protein.hpp"
#include "../structure/Atom.hpp"
#include <vector>
#include <cmath>
#include <iostream>

class Screen {
public:
    Screen(int width, int height);
    void set_protein(Protein* protein);
    void drawScreen();
    bool handle_input();

private:
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    float ASPECT_RATIO;
    char* mScreen;
    Protein* mProtein;

    void project();
    void clear_screen();
    void print_screen();
};