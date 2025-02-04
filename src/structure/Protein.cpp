#include "Protein.hpp"
#include "Atom.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

Protein::Protein(const std::string& in_file, const std::string& chains, const bool& show_structure) {
    load_data(in_file, chains, show_structure); 
    return;
}

std::unordered_map<char, std::unordered_map<int, Atom>>& Protein::get_init_atoms() {
    return init_atoms;  
}

std::unordered_map<char, std::unordered_map<int, Atom>>& Protein::get_on_screen_atoms() {
    return on_screen_atoms; 
}

int Protein::get_length(){
    return init_atoms.size();
}

void Protein::load_ca(const std::string& in_file, const std::string& chains){
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            char chainID = line[21];
            if (chains == "" || chains.find(chainID) == std::string::npos) {
                int idx = std::stoi(line.substr(24, 4));
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));
                Atom new_atom;
                new_atom.set_position(x, y, z);

                init_atoms[chainID][idx] = new_atom;
                on_screen_atoms[chainID][idx] = new_atom;
            }
        }
    }
    openFile.close();
    return;
}

void Protein::load_structure(const std::string& in_file, const std::string& chains) {
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        if (line.substr(0, 5) == "HELIX" || line.substr(0, 5) == "SHEET") {
            char start_chainID, end_chainID;
            int start, end;

            if (line.substr(0, 5) == "SHEET") { // SHEET
                start_chainID = line[21];
                start = std::stoi(line.substr(22, 4));
                end_chainID = line[32];
                end = std::stoi(line.substr(33, 4));
                
                if (start_chainID == end_chainID) {
                    auto& initChain = init_atoms[start_chainID];
                    auto& screenChain = on_screen_atoms[start_chainID];

                    for (int i = start; i <= end; i++) {
                        if (initChain.find(i) != initChain.end()) {
                            initChain[i].set_structure('b');
                        }
                        if (screenChain.find(i) != screenChain.end()) {
                            screenChain[i].set_structure('b');
                        }
                    }
                }
            }
            else if (line.substr(0, 5) == "HELIX") {
                start_chainID = line[19];
                start = std::stoi(line.substr(21, 4));
                end_chainID = line[31];
                end = std::stoi(line.substr(33, 4));

                if (start_chainID == end_chainID) {
                    auto& initChain = init_atoms[start_chainID];
                    auto& screenChain = on_screen_atoms[start_chainID];

                    for (int i = start; i <= end; i++) {
                        if (initChain.find(i) != initChain.end()) {
                            initChain[i].set_structure('a');
                        }
                        if (screenChain.find(i) != screenChain.end()) {
                            screenChain[i].set_structure('a');
                        }
                    }
                }
            } 
        }
    }
}


void Protein::load_data(const std::string& in_file, const std::string& chains, const bool& show_structure) {
    load_ca(in_file, chains);
    if (show_structure) {
        load_structure(in_file, chains);
    }

    return;
}

void Protein::set_rotate(int x_rotate, int y_rotate, int z_rotate){
    const float PI = 3.14159265359;
    const float UNIT = 12;
    std::cout << "Rotate: " << x_rotate << " " << y_rotate << " " << z_rotate << std::endl;

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

    std::cout << "Rotate done" << std::endl;

    return;
}

void Protein::do_rotation(float rotate_mat[3][3]) {
    int len = get_length();
    // simd_float u00 = simdf32_set(rotate_mat[0][0]);
    // simd_float u01 = simdf32_set(rotate_mat[0][1]);
    // simd_float u02 = simdf32_set(rotate_mat[0][2]);
    // simd_float u10 = simdf32_set(rotate_mat[1][0]);
    // simd_float u11 = simdf32_set(rotate_mat[1][1]);
    // simd_float u12 = simdf32_set(rotate_mat[1][2]);
    // simd_float u20 = simdf32_set(rotate_mat[2][0]);
    // simd_float u21 = simdf32_set(rotate_mat[2][1]);
    // simd_float u22 = simdf32_set(rotate_mat[2][2]);

    // for (int i = 0; i < len / VECSIZE_FLOAT * VECSIZE_FLOAT; i += VECSIZE_FLOAT) {
    //     simd_float x_x = simdf32_load(&on_screen_atoms[i].mX);
    //     simd_float x_y = simdf32_load(&on_screen_atoms[i].mY);
    //     simd_float x_z = simdf32_load(&on_screen_atoms[i].mZ);

    //     simd_float xx = simdf32_add(simdf32_mul(u00, x_x), simdf32_add(simdf32_mul(u01, x_y), simdf32_mul(u02, x_z)));
    //     simd_float yy = simdf32_add(simdf32_mul(u10, x_x), simdf32_add(simdf32_mul(u11, x_y), simdf32_mul(u12, x_z)));
    //     simd_float zz = simdf32_add(simdf32_mul(u20, x_x), simdf32_add(simdf32_mul(u21, x_y), simdf32_mul(u22, x_z)));

    //     simdf32_store(&on_screen_atoms[i].mX, xx);
    //     simdf32_store(&on_screen_atoms[i].mY, yy);
    //     simdf32_store(&on_screen_atoms[i].mZ, zz);
    // }

    for (auto& [chainID, chain_atoms] : on_screen_atoms) {  
        for (auto& [idx, atom] : chain_atoms) { 
            float x = atom.mX;
            float y = atom.mY;
            float z = atom.mZ;

            atom.mX = rotate_mat[0][0] * x + rotate_mat[0][1] * y + rotate_mat[0][2] * z;
            atom.mY = rotate_mat[1][0] * x + rotate_mat[1][1] * y + rotate_mat[1][2] * z;
            atom.mZ = rotate_mat[2][0] * x + rotate_mat[2][1] * y + rotate_mat[2][2] * z;
        }
    }

    return;
}

void Protein::set_shift(int shift_x, int shift_y, int shift_z) { 
    std::cout << "Shift: " << shift_x << " " << shift_y << " " << shift_z << std::endl;
    float shift_mat[3] = {shift_x, shift_y, shift_z};
    do_shift(shift_mat);
    std::cout << "Shift done" << std::endl;

    return;
}

void Protein::do_shift(float shift_mat[3]) {
    for (auto& [chainID, chain_atoms] : on_screen_atoms) { 
        for (auto& [idx, atom] : chain_atoms) {
            atom.mX += shift_mat[0];
            atom.mY += shift_mat[1];
            atom.mZ += shift_mat[2];
        }
    }
    // int len = on_screen_atoms.size();
    // simd_float t0 = simdf32_set(shift_mat[0]);
    // simd_float t1 = simdf32_set(shift_mat[1]);
    // simd_float t2 = simdf32_set(shift_mat[2]);

    // int simd_end = len - (len % VECSIZE_FLOAT);
    // for (int i = 0; i < len; i += VECSIZE_FLOAT){
    //     simd_float x_x = simdf32_load(&(on_screen_atoms[i].mX));
    //     simd_float x_y = simdf32_load(&(on_screen_atoms[i].mY));
    //     simd_float x_z = simdf32_load(&(on_screen_atoms[i].mZ));
    //     x_x = simdf32_add(x_x, t0);
    //     x_y = simdf32_add(x_y, t1);
        
    //     // simdf32_store(&(on_screen_atoms[i].mX), simdf32_add(x_x, t0));
    //     // simdf32_store(&(on_screen_atoms[i].mY), simdf32_add(x_y, t1));
    //     // simdf32_store(&(on_screen_atoms[i].mZ), simdf32_add(x_z, t2));
    // }

}
