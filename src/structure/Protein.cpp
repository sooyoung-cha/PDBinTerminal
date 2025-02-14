#include "Protein.hpp"
#include "Atom.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

Protein::Protein(const std::string& in_file, const std::string& target_chains, const bool& show_structure) {
    load_data(in_file, target_chains, show_structure); 
    return;
}

Protein::~Protein() {
    for (auto& [chainID, chain_atoms] : init_atoms) {
        delete[] chain_atoms;
    }
    for (auto& [chainID, chain_atoms] : on_screen_atoms) {
        delete[] chain_atoms;
    }
}


std::map<char, Atom*>& Protein::get_init_atoms() {
    return init_atoms;  
}

std::map<char, Atom*>& Protein::get_on_screen_atoms() {
    return on_screen_atoms; 
}

std::map<char, int>& Protein::get_num_chain_Atoms() {
    return num_chain_Atoms; 
}

int& Protein::get_num_chain_Atoms(char chainID) {
    return num_chain_Atoms[chainID]; 
}

int Protein::get_length() {
    int total_atoms = 0;
    for (const auto& [chainID, count] : num_chain_Atoms) {
        total_atoms += count;
    }
    return total_atoms;
}


void Protein::get_cnt_atom_ss(const std::string& in_file, const std::string& target_chains, 
                              std::vector<std::tuple<char, int, char, int, char>>& sec_struct_info,
                              bool show_structure) {
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            char chainID = line[21];
            if (target_chains.empty() || target_chains.find(chainID) != std::string::npos) {
                num_chain_Atoms[chainID]++;
            }
        }
        else if (show_structure && (line.substr(0, 5) == "HELIX" || line.substr(0, 5) == "SHEET")) {
            char start_chainID, end_chainID, struct_type;
            int start, end;

            if (line.substr(0, 5) == "SHEET") { // SHEET (β-strand)
                start_chainID = line[21];
                start = std::stoi(line.substr(22, 4));
                end_chainID = line[32];
                end = std::stoi(line.substr(33, 4));
                struct_type = 'S'; // SHEET → 'S'
            }
            else if (line.substr(0, 5) == "HELIX") { // HELIX (α-helix)
                start_chainID = line[19];
                start = std::stoi(line.substr(21, 4));
                end_chainID = line[31];
                end = std::stoi(line.substr(33, 4));
                struct_type = 'H'; // HELIX → 'H'
            }

            sec_struct_info.emplace_back(start_chainID, start, end_chainID, end, struct_type);
        }
    }
    openFile.close();
}

void Protein::load_ca(const std::string& in_file, const std::string& target_chains,
                      std::vector<std::tuple<char, int, char, int, char>> sec_struct_info,
                      bool show_structure) {
    for (const auto& [chain, num_atoms] : num_chain_Atoms) {
        init_atoms[chain] = new Atom[num_atoms];
        on_screen_atoms[chain] = new Atom[num_atoms];
    }

    std::map<char, int> atom_counter; // 각 체인의 현재 저장 위치
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            char chainID = line[21];
            if (target_chains.empty() || target_chains.find(chainID) != std::string::npos) {
                int idx = atom_counter[chainID]++; // 0부터 연속적인 index로 저장
                int pdb_idx = std::stoi(line.substr(24, 4)); 
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));

                if (init_atoms.count(chainID) && idx < num_chain_Atoms[chainID]) {
                    init_atoms[chainID][idx].set_position(x, y, z);
                    on_screen_atoms[chainID][idx].set_position(x, y, z);
                    
                    if (show_structure){
                        for (const auto& [start_chainID, start, end_chainID, end, struct_type] : sec_struct_info) {
                            if (chainID == start_chainID && pdb_idx >= start && pdb_idx <= end) {
                                init_atoms[chainID][idx].set_structure(struct_type);
                                on_screen_atoms[chainID][idx].set_structure(struct_type);
                            }
                        }
                    }
                }
            }
        }
    }

    openFile.close();
}


void Protein::load_data(const std::string& in_file, const std::string& target_chains, const bool& show_structure) {
    
    std::vector<std::tuple<char, int, char, int, char>> sec_struct_info; 
    get_cnt_atom_ss(in_file, target_chains, sec_struct_info, show_structure);

    if (num_chain_Atoms.empty()) {
        std::cerr << "Error: num_chain_Atoms is empty. Possible issue in get_cnt_atom_ss()." << std::endl;
        return;
    }

    load_ca(in_file, target_chains, sec_struct_info, show_structure);
}

void Protein::set_rotate(int x_rotate, int y_rotate, int z_rotate){
    const float PI = 3.14159265359;
    const float UNIT = 12;

    if (x_rotate != 0) {
        float rotate_mat[3][3] = {
            {1, 0, 0},
            {0, cos(x_rotate * PI / UNIT), -sin(x_rotate * PI / UNIT)},
            {0, sin(x_rotate * PI / UNIT), cos(x_rotate * PI / UNIT)}
        };
        do_rotation(rotate_mat);
    }
    else if (y_rotate != 0) {
        float rotate_mat[3][3] = {
            {cos(y_rotate * PI / UNIT), 0, sin(y_rotate * PI / UNIT)},
            {0, 1, 0},
            {-sin(y_rotate * PI / UNIT), 0, cos(y_rotate * PI / UNIT)}
        };
        do_rotation(rotate_mat);
    }
    else if (z_rotate != 0) {
        float rotate_mat[3][3] = {
          {cos(z_rotate * PI / UNIT), -sin(z_rotate * PI / UNIT), 0},
          {sin(z_rotate * PI / UNIT), cos(z_rotate * PI / UNIT), 0},
          {0, 0, 1}
        };
        do_rotation(rotate_mat);
    }

}


void Protein::set_shift(int shift_x, int shift_y, int shift_z) { 
    float shift_mat[3] = {shift_x, shift_y, shift_z};
    do_shift(shift_mat);
}

void Protein::do_rotation(float rotate_mat[3][3]) {
    for (auto& [chainID, chain_atoms] : on_screen_atoms) {  
        if (!chain_atoms) continue;  // nullptr 체크

        int num_atoms = num_chain_Atoms[chainID]; // 해당 체인의 원자 개수
        for (int i = 0; i < num_atoms; ++i) { 
            float x = chain_atoms[i].mX;
            float y = chain_atoms[i].mY;
            float z = chain_atoms[i].mZ;

            chain_atoms[i].mX = rotate_mat[0][0] * x + rotate_mat[0][1] * y + rotate_mat[0][2] * z;
            chain_atoms[i].mY = rotate_mat[1][0] * x + rotate_mat[1][1] * y + rotate_mat[1][2] * z;
            chain_atoms[i].mZ = rotate_mat[2][0] * x + rotate_mat[2][1] * y + rotate_mat[2][2] * z;
        }
    }
}



void Protein::do_shift(float shift_mat[3]) {
    for (auto& [chainID, chain_atoms] : on_screen_atoms) {  
        if (!chain_atoms) continue;  // nullptr 체크

        int num_atoms = num_chain_Atoms[chainID]; // 해당 체인의 원자 개수
        for (int i = 0; i < num_atoms; ++i) {
            chain_atoms[i].mX += shift_mat[0];
            chain_atoms[i].mY += shift_mat[1];
            chain_atoms[i].mZ += shift_mat[2];
        }
    }
}


