#pragma once

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>
#include "Atom.hpp"
#include "StructureMaker.hpp"

class Protein {
public:
    Protein(const std::string& in_file, const std::string& target_chains, const bool& show_structure);
    ~Protein();

    // 데이터 접근자
    std::map<char, std::vector<Atom>>& get_atoms();
    std::map<char, int> get_chain_length();
    int get_chain_length(char chainID);
    int get_length();

    // 초기화

    void load_data(const std::string& in_file, const std::string& target_chains, const bool& show_structure);

    // 변형
    void set_rotate(int x_rotate, int y_rotate, int z_rotate);
    void set_shift(float shift_x, float shift_y, float shift_z);
    void do_rotation(float rotate_mat[3][3]);
    void do_shift(float shift_mat[3]);

private:
    void set_ss_info_pdb(const std::string& in_file, const std::string& target_chains,
                     std::vector<std::tuple<char, int, char, int, char>>& ss_info);

    void set_init_atoms_pdb(const std::string& in_file, 
                        const std::string& target_chains,
                        std::vector<std::tuple<char, int, char, int, char>> ss_info,
                        const bool& show_structure);
    void set_ss_info_cif(const std::string& in_file, const std::string& target_chains,
                         std::vector<std::tuple<char, int, char, int, char>>& ss_info);

    void set_init_atoms_cif(const std::string& in_file, 
                            const std::string& target_chains,
                            std::vector<std::tuple<char, int, char, int, char>> ss_info,
                            const bool& show_structure);
    // 멤버 데이터
    std::map<char, std::vector<Atom>> init_atoms;
    std::map<char, std::vector<Atom>> ss_atoms;
    std::map<char, std::vector<Atom>> screen_atoms;

    StructureMaker structureMaker;
};

