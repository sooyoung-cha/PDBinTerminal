#include "Protein.hpp"
#include "Atom.hpp"
#include <fstream>
#include <iostream>
#include <cmath>

Protein::Protein(const std::string& in_file) {
    load_data(in_file); 
    return;
}

std::vector<Atom>& Protein::get_init_atoms() {
    return init_atoms;  
}

std::vector<Atom>& Protein::get_on_screen_atoms() {
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
            if (chains == "" || chains.find(line[21]) == std::string::npos) {
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));
                Atom new_atom;
                new_atom.mX = x;
                new_atom.mY = y;
                new_atom.mZ = z;

                init_atoms.push_back(new_atom);
                on_screen_atoms.push_back(new_atom);
            }
        }
    }
    openFile.close();
    return;
}

void Protein::load_structure(const std::string& in_file, const std::string& chains){
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "HELIX" || line.substr(0, 4) == "SHEET"){
            int start = std::stoi(line.substr(21, 4));
            int end = std::stoi(line.substr(33, 4));
            char init_chain = line[19];
            char end_chain = line[31];

            if (line.substr(0, 4) == "HELIX") {
                for (int i = start; i <= end; i++) {
                    
                }
            }
            else {
                for (int i = start; i <= end; i++) {
                }
            }
        }
    }
}

void Protein::load_data(const std::string& in_file, const std::string& chains, const bool& show_structure) {
    load_ca(in_file, chains);
    if (show_structure) {
        load_structure(in_file, chains)
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

    for (int i = 0; i < len; ++i) {
        float x = on_screen_atoms[i].mX;
        float y = on_screen_atoms[i].mY;
        float z = on_screen_atoms[i].mZ;
        on_screen_atoms[i].mX = rotate_mat[0][0] * x + rotate_mat[0][1] * y + rotate_mat[0][2] * z;
        on_screen_atoms[i].mY = rotate_mat[1][0] * x + rotate_mat[1][1] * y + rotate_mat[1][2] * z;
        on_screen_atoms[i].mZ = rotate_mat[2][0] * x + rotate_mat[2][1] * y + rotate_mat[2][2] * z;
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
    int len = get_length();
    for (int i = 0; i < len; ++i) { 
        on_screen_atoms[i].mX += shift_mat[0];
        on_screen_atoms[i].mY += shift_mat[1];
        on_screen_atoms[i].mZ += shift_mat[2];
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
