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

struct BoundingBox {
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();
    float max_z = std::numeric_limits<float>::lowest();

    void expand(float x, float y, float z) {
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        min_y = std::min(min_y, y);
        max_y = std::max(max_y, y);
        min_z = std::min(min_z, z);
        max_z = std::max(max_z, z);
    }

    BoundingBox operator+(const BoundingBox& other) const {
        BoundingBox result;
        result.min_x = std::min(min_x, other.min_x);
        result.min_y = std::min(min_y, other.min_y);
        result.min_z = std::min(min_z, other.min_z);
        result.max_x = std::max(max_x, other.max_x);
        result.max_y = std::max(max_y, other.max_y);
        result.max_z = std::max(max_z, other.max_z);
        return result;
    }
};

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

    void load_data();

    // 변형
    void set_rotate(int x_rotate, int y_rotate, int z_rotate);
    void set_shift(float shift_x, float shift_y, float shift_z);
    void do_rotation(float rotate_mat[3][3]);
    void do_shift(float shift_mat[3]);

    const BoundingBox& get_bounding_box() const { return bounding_box; }
    void set_bounding_box(const BoundingBox& bb) { bounding_box = bb; }

private:
    void set_bbox_pdb(const std::string& in_file);
    void set_ss_info_pdb(const std::string& in_file, 
                         const std::string& target_chains,
                         std::vector<std::tuple<char, int, char, int, char>>& ss_info);
    void set_init_atoms_pdb(const std::string& in_file, 
                            const std::string& target_chains,
                            std::vector<std::tuple<char, int, char, int, char>> ss_info);

    void set_bbox_cif(const std::string& in_file);
    void set_ss_info_cif(const std::string& in_file, 
                         const std::string& target_chains,
                         std::vector<std::tuple<char, int, char, int, char>>& ss_info);
    void set_init_atoms_cif(const std::string& in_file, 
                            const std::string& target_chains,
                            std::vector<std::tuple<char, int, char, int, char>> ss_info);
    // 멤버 데이터
    std::map<char, std::vector<Atom>> init_atoms;
    std::map<char, std::vector<Atom>> ss_atoms;
    std::map<char, std::vector<Atom>> screen_atoms;

    std::string in_file;
    std::string target_chains;
    bool show_structure;

    BoundingBox bounding_box;

    StructureMaker structureMaker;
};

