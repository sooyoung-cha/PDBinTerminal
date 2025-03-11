#pragma once
#include "../structure/Protein.hpp"
#include "../structure/Atom.hpp"
#include <vector>
#include <cmath>
#include <iostream>

class Screen {
public:
    Screen(const int& width, const int& height, const bool& show_structure);
    ~Screen();
    void set_protein(Protein* protein);
    void set_zoom_level(float zoom);
    bool handle_input();
    void drawScreen();
    void initialize_colors();
    void drawLine(std::vector<char>& buffer, int x1, int y1, int x2, int y2, int width, float z1, float z2);

private:
    int screen_width;
    int screen_height;
    bool screen_show_structure;
    float aspect_ratio;
    float zoom_level;
    char* mScreen;
    std::unordered_map<char, std::vector<char>> screen_buffer_by_chain;  // ✅ 체인별 화면 버퍼
    Protein* data;

    std::map<char, int> chain_colors;

    void project();
    void clear_screen();
    void print_screen();
};