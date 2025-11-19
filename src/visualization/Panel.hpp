#pragma once
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

class Panel {
public:
    Panel(int width);

    void add_panel_info(const std::string& file_name, const std::map<char, int>& chain_info, const std::map<char, int>& chain_residue_info);

    std::string get_panel_info() const;

private:
    std::map<std::string, std::map<char, int>> panel_atom_info;
    std::map<std::string, std::map<char, int>> panel_residue_info;
    int panel_width;
};