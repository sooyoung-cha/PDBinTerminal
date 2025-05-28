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

void Protein::set_ss_info_pdb(const std::string& in_file, const std::string& target_chains, 
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

void Protein::set_init_atoms_pdb(const std::string& in_file, 
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

void Protein::set_ss_info_cif(const std::string& in_file, const std::string& target_chains,
                              std::vector<std::tuple<char, int, char, int, char>>& ss_info) {
    auto parse_section = [&](const std::string& keyword, char struct_type) {
        std::ifstream file(in_file);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << in_file << std::endl;
            return;
        }

        std::string line;
        bool loop = false, in_section = false;
        std::vector<std::string> field_order;
        int str_site_idx = -1, start_chain_idx = -1, end_chain_idx = -1, start_idx = -1, end_idx = -1;
        char start_chain, end_chain;
        int start, end;

        while (std::getline(file, line)) {
            if (line.find("loop_") != std::string::npos) {
                loop = true;
            } 
            else if (line.find("#") != std::string::npos && !in_section){
                loop = false;
            }
            else if (line.find("#") != std::string::npos && in_section) {
                if (!loop){
                    if (target_chains.empty() || target_chains.find(start_chain) != std::string::npos) {
                        ss_info.emplace_back(start_chain, start, end_chain, end, struct_type);
                    }
                }
                break;
            } 
            else if (line.find(keyword) != std::string::npos) {
                in_section = true;
                if (loop) {
                    str_site_idx++;
                    if (line.find("beg_auth_asym_id") != std::string::npos){
                        start_chain_idx = str_site_idx;
                    }
                    else if (line.find("beg_auth_seq_id") != std::string::npos){
                        start_idx = str_site_idx;
                    }
                    else if (line.find("end_auth_asym_id") != std::string::npos){
                        end_chain_idx = str_site_idx;
                    }
                    else if (line.find("end_auth_seq_id") != std::string::npos){
                        end_idx = str_site_idx;
                    }
                }
                else {
                    std::istringstream ss(line);
                    std::string token;
                    std::vector<std::string> tokens;
                    while (ss >> token) tokens.push_back(token);

                    if (line.find("beg_auth_asym_id") != std::string::npos){
                        start_chain = tokens[2][0];
                    }
                    else if (line.find("beg_auth_seq_id") != std::string::npos){
                        start = std::stoi(tokens[2]);
                    }
                    else if (line.find("end_auth_asym_id") != std::string::npos){
                        end_chain = tokens[2][0];
                    }
                    else if (line.find("end_auth_seq_id") != std::string::npos){
                        end = std::stoi(tokens[2]);
                    }
                }
            } 
            else if (in_section && loop) {
                std::istringstream ss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (ss >> token) tokens.push_back(token);
                start_chain = tokens[start_chain_idx][0];
                end_chain = tokens[end_chain_idx][0];
                start = std::stoi(tokens[start_idx]);
                end = std::stoi(tokens[end_idx]);
                
                if (target_chains.empty() || target_chains.find(start_chain) != std::string::npos) {
                    ss_info.emplace_back(start_chain, start, end_chain, end, struct_type);
                }
            }
        }
        file.close();
    };

    // 두 섹션을 각각 파싱
    parse_section("_struct_conf.", 'H');             // 헬릭스
    parse_section("_struct_sheet_range.", 'S');      // 시트
}


void Protein::set_init_atoms_cif(const std::string& in_file,
                                 const std::string& target_chains,
                                 std::vector<std::tuple<char, int, char, int, char>> ss_info,
                                 const bool& show_structure) {

    std::ifstream file(in_file);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> atom_lines;
    int atom_site_idx = -1;

    int group_PDB_idx, x_idx, y_idx, z_idx, chain_idx, ca_idx, id_idx;

    float min_x = std::numeric_limits<float>::max(), min_y = min_x, min_z = min_x;
    float max_x = std::numeric_limits<float>::lowest(), max_y = max_x, max_z = max_x;
    std::vector<std::tuple<char, int, float, float, float>> atom_data;

    // ✅ 1. atom_site 영역 수집
    while (std::getline(file, line)) {
        if (line.find("_atom_site.") != std::string::npos) {
            atom_site_idx++;
            auto pos = line.rfind('.');
            if (line.substr(pos + 1).find("group_PDB") != std::string::npos){
                group_PDB_idx = atom_site_idx;
            }
            else if (line.substr(pos + 1).find("Cartn_x") != std::string::npos){
                x_idx = atom_site_idx;
            }
            else if (line.substr(pos + 1).find("Cartn_y") != std::string::npos){
                y_idx = atom_site_idx;
            }
            else if (line.substr(pos + 1).find("Cartn_z") != std::string::npos){
                z_idx = atom_site_idx;
            }
            else if (line.substr(pos + 1).find("auth_asym_id") != std::string::npos){
                chain_idx = atom_site_idx;
            }
            else if (line.substr(pos + 1).find("label_atom_id") != std::string::npos){
                ca_idx = atom_site_idx;
            }
            else if (line.substr(pos + 1).find("auth_seq_id") != std::string::npos){
                id_idx = atom_site_idx;
            }
        }
        else if (atom_site_idx != -1){
            if (line.find("#") != std::string::npos) {
                break;
            }
            
            std::istringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            while (ss >> token) {
                tokens.push_back(token);
            }
            if (tokens[group_PDB_idx] == "ATOM" && tokens[ca_idx] == "CA"){
                float x = std::stof(tokens[x_idx]);
                float y = std::stof(tokens[y_idx]);
                float z = std::stof(tokens[z_idx]);
                int id = std::stoi(tokens[id_idx]);
                char chainID = tokens[chain_idx][0];
        
                atom_data.emplace_back(chainID, id, x, y, z);
        
                min_x = std::min(min_x, x); max_x = std::max(max_x, x);
                min_y = std::min(min_y, y); max_y = std::max(max_y, y);
                min_z = std::min(min_z, z); max_z = std::max(max_z, z);
            }
        }
    }
    file.close();

    // ✅ 2. 정규화된 좌표 저장
    for (const auto& [chainID, id, x, y, z] : atom_data) {
        float atom_x = 2 * (x - min_x) / (max_x - min_x) - 1;
        float atom_y = 2 * (y - min_y) / (max_y - min_y) - 1;
        float atom_z = 2 * (z - min_z) / (max_z - min_z) - 1;

        
        Atom new_atom(atom_x, atom_y, atom_z);
        for (const auto& [start_chainID, start, end_chainID, end, struct_type] : ss_info) {
            if (chainID == start_chainID && id >= start && id <= end) {
                new_atom.set_structure(struct_type);
                std::cout << chainID << " " << id << " " << struct_type << std::endl;
            }
        }
        
        init_atoms[chainID].push_back(new_atom);
    }
}

std::ostream& operator<<(std::ostream& os, const std::tuple<char, int, char, int, char>& t) {
    os << "("
       << std::get<0>(t) << ", "
       << std::get<1>(t) << ", "
       << std::get<2>(t) << ", "
       << std::get<3>(t) << ", "
       << std::get<4>(t) << ")";
    return os;
}

void Protein::load_data(const std::string& in_file, const std::string& target_chains, const bool& show_structure) {
    
    std::vector<std::tuple<char, int, char, int, char>> ss_info;
    if (in_file.find(".pdb") != std::string::npos) {
        set_ss_info_pdb(in_file, target_chains, ss_info);
        set_init_atoms_pdb(in_file, target_chains, ss_info, show_structure);

        if (init_atoms.empty()) {
            std::cerr << "Error: input PDB file is empty." << std::endl;
            return;
        }

        if (show_structure){
            structureMaker.calculate_ss_points(init_atoms, ss_atoms);
            screen_atoms = ss_atoms;
        }
        else{ screen_atoms = init_atoms; }
    }
    else if (in_file.find(".cif") != std::string::npos){
        set_ss_info_cif(in_file, target_chains, ss_info);
        set_init_atoms_cif(in_file, target_chains, ss_info, show_structure);

        if (init_atoms.empty()) {
            std::cerr << "Error: input CIF file is empty." << std::endl;
            return;
        }

        if (show_structure){
            structureMaker.calculate_ss_points(init_atoms, ss_atoms);
            screen_atoms = ss_atoms;
        }
        else{ screen_atoms = init_atoms; }
    }
    else{
        std::cerr << "Error: input file format is not supported." << std::endl;
        return;
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