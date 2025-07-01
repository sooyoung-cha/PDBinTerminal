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
    bool handle_input();
    bool isSame = true;
    char getPixelCharFromDepth(float z);
    void set_protein(const std::string& in_file, const std::string& target_chains, const bool& show_structure);
    void normalize_proteins();
    void set_utmatrix(int protein_idx, const std::string& umatrix, const std::string& tmatrix);
    void set_zoom_level(float zoom);
    void drawScreen();
    void assign_colors_to_points(std::vector<RenderPoint>& points, int protein_idx);
    void drawLine(std::vector<RenderPoint>& points,
                  int x1, int y1, int x2, int y2,
                  float z1, float z2, char chainID, char structure);
    // void drawAlphHelix(std::vector<RenderPoint>& points);
    // void drawBetaSheet(std::vector<RenderPoint>& points);

private:
    int structNum = -1;
    int screen_width;
    int screen_height;
    bool screen_show_structure;
    std::string screen_mode;
    float aspect_ratio;
    std::vector<ScreenPixel> screenPixels;  
    std::vector<Protein*> data;  
    std::vector<float> zoom_level;
    BoundingBox global_bb;

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

        std::vector<int> unrainbow_ids = {
        82,  // Green
        33,  // Blue
        202, // Orange
        93,  // Violet
        226, // Yellow
        118, // Lime
    };
};