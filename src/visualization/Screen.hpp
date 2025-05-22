#pragma once
#include "../structure/Protein.hpp"
#include "../structure/Atom.hpp"
#include <vector>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <ncurses.h>

struct RenderPoint {
    int x, y;
    float z;
    char pixel;
    char chainID;
    char structure = 'x';
    int color_id = 0;  
};

struct ScreenPixel {
    float depth = std::numeric_limits<float>::max();  // z-buffer용
    char pixel = '_';      // 표시할 문자
    int color_id = 0;      // ncurses color pair index
};

class Screen {
public:
    Screen(const int& width, const int& height, const bool& show_structure, const std::string& mode);
    ~Screen();
    void set_protein1(Protein* protein);
    void set_protein2(Protein* protein);
    void set_zoom_level(float zoom);
    bool handle_input();
    void drawScreen();
    void assign_colors_to_points(std::vector<RenderPoint>& points);
    void drawLine(std::vector<RenderPoint>& points,
                  int x1, int y1, int x2, int y2,
                  float z1, float z2, char chainID, char structure);
    void drawAlphHelix(std::vector<RenderPoint>& points);
    void drawBetaSheet(std::vector<RenderPoint>& points);
    char getPixelCharFromDepth(float z);

private:
    int screen_width;
    int screen_height;
    bool screen_show_structure;
    std::string screen_mode;
    float aspect_ratio;
    float zoom_level;
    std::vector<ScreenPixel> screenPixels;  
    Protein* data1;
    Protein* data2;

    std::unordered_map<char, int> chain_colors;

    void project();
    void clear_screen();
    void print_screen();

    std::vector<int> rainbow_ids = {
        196, // Red
        202, // Orange
        208, // Dark Orange
        214, // Light Orange
        220, // Gold
        226, // Yellow
        190, // Light Yellow-Green
        154, // Chartreuse
        118, // Lime
        82,  // Green
        46,  // Spring Green
        49,  // Aqua Green
        51,  // Cyan
        45,  // Sky Blue
        39,  // Dodger Blue
        33,  // Blue
        27,  // Dark Blue
        21,  // Navy
        93,  // Violet
        129  // Magenta
    };
};