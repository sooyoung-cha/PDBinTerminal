#include "Protein.hpp"

Protein::Protein(const std::string& in_file, const std::string& target_chains, const bool& show_structure) {
    structureMaker = StructureMaker();
    load_data(in_file, target_chains, show_structure); 

    return;
}

Protein::~Protein() {
}


std::map<char, std::vector<Atom>>& Protein::get_atoms() {
    return screen_atoms;  
}

std::map<char, int> Protein::get_chain_length() {
    std::map<char, int> result;
    for (const auto& [chainID, atoms] : screen_atoms) {
        result[chainID] = atoms.size();
    }
    return result;
}
int Protein::get_chain_length(char chainID) {
    if (screen_atoms.count(chainID)) {
        return screen_atoms[chainID].size();
    }
    return 0;
}

int Protein::get_length() {
    int total_atoms = 0;
    for (const auto& [chainID, atoms] : screen_atoms) {
        total_atoms += atoms.size();
    }
    return total_atoms;
}

void Protein::set_ss_info(const std::string& in_file, const std::string& target_chains, 
                          std::vector<std::tuple<char, int, char, int, char>>& ss_info) {
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {        
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

        ss_info.emplace_back(start_chainID, start, end_chainID, end, struct_type);
    }
    openFile.close();
}

void Protein::set_init_atoms(const std::string& in_file, 
                             const std::string& target_chains,
                             std::vector<std::tuple<char, int, char, int, char>> ss_info,
                             const bool& show_structure) {

    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    // ✅ 1. 최소/최대 좌표 찾기
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float min_z = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();
    float max_z = std::numeric_limits<float>::lowest();

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            float x = std::stof(line.substr(30, 8));
            float y = std::stof(line.substr(38, 8));
            float z = std::stof(line.substr(46, 8));

            min_x = std::min(min_x, x);
            min_y = std::min(min_y, y);
            min_z = std::min(min_z, z);
            max_x = std::max(max_x, x);
            max_y = std::max(max_y, y);
            max_z = std::max(max_z, z);
        }
    }

    openFile.clear();
    openFile.seekg(0); // 파일 다시 읽기 준비

    // ✅ 2. 정규화된 좌표 저장
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            char chainID = line[21];
            if (target_chains.empty() || target_chains.find(chainID) != std::string::npos) {
                int pdb_idx = std::stoi(line.substr(24, 4)); 
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));

                // ✅ 좌표 정규화 ([-1, 1] 범위로 조정)
                x = 2 * (x - min_x) / (max_x - min_x) - 1;
                y = 2 * (y - min_y) / (max_y - min_y) - 1;
                z = 2 * (z - min_z) / (max_z - min_z) - 1;

                Atom new_atom(x, y, z);
                
                for (const auto& [start_chainID, start, end_chainID, end, struct_type] : ss_info) {
                    if (chainID == start_chainID && pdb_idx >= start && pdb_idx <= end) {
                        new_atom.set_structure(struct_type);
                    }
                }
                
                init_atoms[chainID].push_back(new_atom);
            }
        }
    }
    openFile.close();

}

void Protein::load_data(const std::string& in_file, const std::string& target_chains, const bool& show_structure) {
    
    std::vector<std::tuple<char, int, char, int, char>> ss_info; 
    set_ss_info(in_file, target_chains, ss_info);
    set_init_atoms(in_file, target_chains, ss_info, show_structure);

    if (init_atoms.empty()) {
        std::cerr << "Error: input PDB file is empty." << std::endl;
        return;
    }

    if (show_structure){
        structureMaker.calculate_ss_points(init_atoms, ss_atoms);
        screen_atoms = ss_atoms;
    }
    else{
        screen_atoms = init_atoms;
    }
}

void Protein::set_rotate(int x_rotate, int y_rotate, int z_rotate){
    const float PI = 3.14159265359;
    // const float UNIT = 12;
    const float UNIT = 48;

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


void Protein::set_shift(float shift_x, float shift_y, float shift_z) { 
    float shift_mat[3] = {shift_x, shift_y, shift_z};
    do_shift(shift_mat);
}

void Protein::do_rotation(float rotate_mat[3][3]) {
    for (auto& [chainID, chain_atoms] : screen_atoms) {
        for (Atom& atom : chain_atoms) {
            float x = atom.mX;
            float y = atom.mY;
            float z = atom.mZ;

            atom.mX = rotate_mat[0][0] * x + rotate_mat[0][1] * y + rotate_mat[0][2] * z;
            atom.mY = rotate_mat[1][0] * x + rotate_mat[1][1] * y + rotate_mat[1][2] * z;
            atom.mZ = rotate_mat[2][0] * x + rotate_mat[2][1] * y + rotate_mat[2][2] * z;
        }
    }
}

void Protein::do_shift(float shift_mat[3]) {
    for (auto& [chainID, chain_atoms] : screen_atoms) {
        for (Atom& atom : chain_atoms) {
            atom.mX += shift_mat[0];
            atom.mY += shift_mat[1];
            atom.mZ += shift_mat[2];
        }
    }
}