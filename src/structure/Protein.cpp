#include "Protein.hpp"

Protein::Protein(const std::string& in_file_, const std::string& target_chains_, const bool& show_structure_) {
    structureMaker = StructureMaker();

    in_file = in_file_;
    target_chains = target_chains_;
    show_structure = show_structure_;
    cx = cy = cz = scale = 0.0;

    if (in_file.find(".pdb") != std::string::npos) 
        load_bbox_pdb(in_file);
    else if (in_file.find(".cif") != std::string::npos)
        load_bbox_cif(in_file);
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

float Protein::get_scaled_min_z() { 
    return (bounding_box.min_z - cz) * scale; 
}

float Protein::get_scaled_max_z() { 
    return (bounding_box.max_z - cz) * scale; 
}

BoundingBox& Protein::get_bounding_box() {
     return bounding_box; 
}

void Protein::set_scale(float scale_) { 
    scale = scale_;
    cx = 0.5f * (bounding_box.min_x + bounding_box.max_x);
    cy = 0.5f * (bounding_box.min_y + bounding_box.max_y);
    cz = 0.5f * (bounding_box.min_z + bounding_box.max_z);
    ssPredictor.set_scale(1.0f/scale);
}    

// pdb file
bool Protein::is_ss_in_pdb(const std::string& in_file){
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return false;
    }

    std::string line;
    while (getline(openFile, line)) {        
        if (line.find("SHEET") != std::string::npos || line.find("HELIX") != std::string::npos) { 
            openFile.close();
            return true;
        }
    }

    openFile.close();
    return false;
}

void Protein::load_bbox_pdb(const std::string& in_file) {
    std::cout << "  calculate protein bounding box\n";    
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            char chainID = line[21];
            if (target_chains == "-" || target_chains.find(chainID) != std::string::npos) {
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));

                bounding_box.min_x = std::min(bounding_box.min_x, x);
                bounding_box.min_y = std::min(bounding_box.min_y, y);
                bounding_box.min_z = std::min(bounding_box.min_z, z);
                bounding_box.max_x = std::max(bounding_box.max_x, x);
                bounding_box.max_y = std::max(bounding_box.max_y, y);
                bounding_box.max_z = std::max(bounding_box.max_z, z);            
            }
        }
    }
    openFile.close();
}

void Protein::load_init_atoms_pdb(const std::string& in_file, 
                                  const std::string& target_chains,
                                  const std::vector<std::tuple<char, int, char, int, char>>& ss_info, 
                                 float * vectorpointers , bool yesUT) {
    std::cout << "  load atoms from file with structure\n";    
    std::ifstream openFile(in_file);

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            char chainID = line[21];
            if (target_chains == "-" || target_chains.find(chainID) != std::string::npos) {
                int pdb_idx = std::stoi(line.substr(24, 4)); 
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));

                // ✅ 좌표 정규화 ([-1, 1] 범위로 조정)
                if (yesUT == false) {
                    x = (x - cx) * scale;
                    y = (y - cy) * scale;
                    z = (z - cz) * scale;
                } else {
                    // x = (x + vectorpointers[0]- cx) * scale;
                    // y = (y + vectorpointers[0]- cy) * scale;
                    // z = (z + vectorpointers[0]- cz) * scale;

                    x = (x - cx) * scale;
                    y = (y - cy) * scale;
                    z = (z - cz) * scale;
                }
                
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

void Protein::load_init_atoms_pdb(const std::string& in_file, 
                                  const std::string& target_chains, float * vectorpointers, bool yesUT) {
    std::cout << "  load atoms from file\n";    
    std::ifstream openFile(in_file);

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            char chainID = line[21];
            if (target_chains == "-" || target_chains.find(chainID) != std::string::npos) {
                int pdb_idx = std::stoi(line.substr(24, 4)); 
                float x = std::stof(line.substr(30, 8));
                float y = std::stof(line.substr(38, 8));
                float z = std::stof(line.substr(46, 8));
                if (yesUT == false) {
                    x = (x - cx) * scale;
                    y = (y - cy) * scale;
                    z = (z - cz) * scale;
                } else {
                    // x = (x + vectorpointers[0]- cx) * scale;
                    // y = (y + vectorpointers[0]- cy) * scale;
                    // z = (z + vectorpointers[0]- cz) * scale;

                    x = (x - cx) * scale;
                    y = (y - cy) * scale;
                    z = (z - cz) * scale;
                }
                
                Atom new_atom(x, y, z, 'x');

                init_atoms[chainID].push_back(new_atom);
            }
        }
    }
    openFile.close();

}

void Protein::load_ss_info_pdb(const std::string& in_file,
                              const std::string& target_chains,
                              std::vector<std::tuple<char, int, char, int, char>>& ss_info) {
    std::cout << "  load structures from file\n";    
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
            if (target_chains == "-" || target_chains.find(start_chainID) != std::string::npos) {
                ss_info.emplace_back(start_chainID, start, end_chainID, end, struct_type);
            }
        }
        else if (line.substr(0, 5) == "HELIX") { // HELIX (α-helix)
            start_chainID = line[19];
            start = std::stoi(line.substr(21, 4));
            end_chainID = line[31];
            end = std::stoi(line.substr(33, 4));
            struct_type = 'H'; // HELIX → 'H'
            if (target_chains == "-" || target_chains.find(start_chainID) != std::string::npos) {
                ss_info.emplace_back(start_chainID, start, end_chainID, end, struct_type);
            }
        }
    }
    openFile.close();
}

// cif file
bool Protein::is_ss_in_cif(const std::string& in_file){
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return false;
    }

    std::string line;
    while (getline(openFile, line)) {        
        if (line.find("_struct_conf.") != std::string::npos || line.find("_struct_sheet_range.") != std::string::npos) { 
            openFile.close();
            return true;
        }
    }

    openFile.close();
    return false;
}

void Protein::load_bbox_cif(const std::string& in_file) {
    std::cout << "  calculate protein bounding box\n";

    std::ifstream file(in_file);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> atom_lines;
    int atom_site_idx = -1;

    int group_PDB_idx, x_idx, y_idx, z_idx, chain_idx, ca_idx, id_idx;

    std::vector<std::tuple<char, int, float, float, float>> atom_data;

    while (std::getline(file, line)) {
        if (line.find("_atom_site.") != std::string::npos) {
            atom_site_idx++;
            auto pos = line.rfind('.');
            if (line.substr(pos + 1).find("group_PDB") != std::string::npos) group_PDB_idx = atom_site_idx;
            else if (line.substr(pos + 1).find("Cartn_x") != std::string::npos) x_idx = atom_site_idx;
            else if (line.substr(pos + 1).find("Cartn_y") != std::string::npos) y_idx = atom_site_idx;
            else if (line.substr(pos + 1).find("Cartn_z") != std::string::npos) z_idx = atom_site_idx;
            else if (line.substr(pos + 1).find("auth_asym_id") != std::string::npos) chain_idx = atom_site_idx;
            else if (line.substr(pos + 1).find("label_atom_id") != std::string::npos) ca_idx = atom_site_idx;
        }
        else if (atom_site_idx != -1){
            if (line.find("#") != std::string::npos)  break;
            
            std::istringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            while (ss >> token) {
                tokens.push_back(token);
            }
            if (tokens[group_PDB_idx] == "ATOM" && tokens[ca_idx] == "CA"){                
                char chainID = tokens[chain_idx][0];
                if (target_chains == "-" || target_chains.find(chainID) != std::string::npos) {
                    float x = std::stof(tokens[x_idx]);
                    float y = std::stof(tokens[y_idx]);
                    float z = std::stof(tokens[z_idx]);
            
                    bounding_box.min_x = std::min(bounding_box.min_x, x);
                    bounding_box.min_y = std::min(bounding_box.min_y, y);
                    bounding_box.min_z = std::min(bounding_box.min_z, z);
                    bounding_box.max_x = std::max(bounding_box.max_x, x);
                    bounding_box.max_y = std::max(bounding_box.max_y, y);
                    bounding_box.max_z = std::max(bounding_box.max_z, z);
                }
            }
        }
    }
    file.close();
}

void Protein::load_init_atoms_cif(const std::string& in_file,
                                 const std::string& target_chains,
                                 const std::vector<std::tuple<char, int, char, int, char>>& ss_info, float * vectorpointers, bool yesUT) {
    std::cout << "  load atoms from file\n";

    std::ifstream file(in_file);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> atom_lines;
    int atom_site_idx = -1;

    int group_PDB_idx, x_idx, y_idx, z_idx, chain_idx, ca_idx, id_idx;

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
                if (target_chains == "-" || target_chains.find(chainID) != std::string::npos)  {
                    float atom_x;
                    float atom_y;
                    float atom_z;
                    if (yesUT == false) {
                        atom_x = (x - cx) * scale;
                        atom_y = (y - cy) * scale;
                        atom_z = (z - cz) * scale;
                    } else {
                        // atom_x = (x + vectorpointers[0]- cx) * scale;
                        // atom_y = (y + vectorpointers[0]- cy) * scale;
                        // atom_z = (z + vectorpointers[0]- cz) * scale;

                        atom_x = (x - cx) * scale;
                        atom_y = (y - cy) * scale;
                        atom_z = (z - cz) * scale;
                    }
                    
                    Atom new_atom(atom_x, atom_y, atom_z);
                    for (const auto& [start_chainID, start, end_chainID, end, struct_type] : ss_info) {
                        if (chainID == start_chainID && id >= start && id <= end) {
                            new_atom.set_structure(struct_type);
                        }
                    }
                    
                    init_atoms[chainID].push_back(new_atom);
                }
            }
        }
    }
    file.close();
}

void Protein::load_init_atoms_cif(const std::string& in_file,
                                 const std::string& target_chains, float * vectorpointers, bool yesUT) {
    std::cout << "  load atoms from file\n";    

    std::ifstream file(in_file);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> atom_lines;
    int atom_site_idx = -1;

    int group_PDB_idx, x_idx, y_idx, z_idx, chain_idx, ca_idx, id_idx;

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
                if (target_chains == "-" || target_chains.find(chainID) != std::string::npos)  {
                    float atom_x;
                    float atom_y;
                    float atom_z;
                    if (yesUT == false) {
                        atom_x = (x - cx) * scale;
                        atom_y = (y - cy) * scale;
                        atom_z = (z - cz) * scale;
                    } else {
                        // atom_x = (x + vectorpointers[0]- cx) * scale;
                        // atom_y = (y + vectorpointers[0]- cy) * scale;
                        // atom_z = (z + vectorpointers[0]- cz) * scale;

                        atom_x = (x - cx) * scale;
                        atom_y = (y - cy) * scale;
                        atom_z = (z - cz) * scale;
                    }
                    
                    Atom new_atom(atom_x, atom_y, atom_z, 'x');
                    
                    init_atoms[chainID].push_back(new_atom);
                }
            }
        }
    }
    file.close();
}

void Protein::load_ss_info_cif(const std::string& in_file, 
                               const std::string& target_chains,
                               std::vector<std::tuple<char, int, char, int, char>>& ss_info) {
    std::cout << "  load structures from file\n";    
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
                    if (target_chains == "-" || target_chains.find(start_chain) != std::string::npos) {
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
                
                if (target_chains == "-" || target_chains.find(start_chain) != std::string::npos) {
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

std::ostream& operator<<(std::ostream& os, const std::tuple<char, int, char, int, char>& t) {
    os << "("
       << std::get<0>(t) << ", "
       << std::get<1>(t) << ", "
       << std::get<2>(t) << ", "
       << std::get<3>(t) << ", "
       << std::get<4>(t) << ")";
    return os;
}

void Protein::load_data(float * vectorpointers, bool yesUT) {    
    // pdb
    if (in_file.find(".pdb") != std::string::npos) {
        if (show_structure){
            if (is_ss_in_pdb(in_file)){
                std::vector<std::tuple<char, int, char, int, char>> ss_info;
                load_ss_info_pdb(in_file, target_chains, ss_info);
                load_init_atoms_pdb(in_file, target_chains, ss_info, vectorpointers, yesUT);
            }
            else{
                load_init_atoms_pdb(in_file, target_chains, vectorpointers, yesUT);
                ssPredictor.run(init_atoms);
            }
        }
        else{
            load_init_atoms_pdb(in_file, target_chains, vectorpointers, yesUT);
        }

        if (init_atoms.empty()) {
            std::cerr << "Error: input PDB file is empty." << std::endl;
            return;
        }

        if (show_structure){
            structureMaker.calculate_ss_points(init_atoms, screen_atoms);
        }
        else{ screen_atoms = init_atoms; }
    }

    // cif
    else if (in_file.find(".cif") != std::string::npos){
        if (show_structure){
            if (is_ss_in_cif(in_file)){
                std::vector<std::tuple<char, int, char, int, char>> ss_info;
                load_ss_info_cif(in_file, target_chains, ss_info);
                load_init_atoms_cif(in_file, target_chains, ss_info, vectorpointers, yesUT);
            }
            else{
                load_init_atoms_cif(in_file, target_chains, vectorpointers, yesUT);
                ssPredictor.run(init_atoms);
            }
        }
        else{
            load_init_atoms_cif(in_file, target_chains, vectorpointers, yesUT);
        }

        if (init_atoms.empty()) {
            std::cerr << "Error: input CIF file is empty." << std::endl;
            return;
        }

        if (show_structure){
            structureMaker.calculate_ss_points(init_atoms, screen_atoms);
        }
        else{ screen_atoms = init_atoms; }
    }

    // others
    else{
        std::cerr << "Error: input file format is not supported." << std::endl;
        return;
    }
    std::cout << std::endl;
}

void Protein::set_rotate(int x_rotate, int y_rotate, int z_rotate){
    const float PI = 3.14159265359;
    // const float UNIT = 12;
    const float UNIT = 48;

    if (x_rotate != 0) {
        float values[9] = {1, 0, 0,
                   0, cos(x_rotate * PI / UNIT), -sin(x_rotate * PI / UNIT), 
                   0, sin(x_rotate * PI / UNIT), cos(x_rotate * PI / UNIT)};

        float* rotate_mat = new float[9];
        for (int i = 0; i < 9; i++) {
            rotate_mat[i] = values[i];
        }
        do_rotation(rotate_mat);
    }
    else if (y_rotate != 0) {
        float values[9] = {cos(y_rotate * PI / UNIT), 0, sin(y_rotate * PI / UNIT),
                   0, 1, 0, 
                   -sin(y_rotate * PI / UNIT), 0, cos(y_rotate * PI / UNIT)};

        float* rotate_mat = new float[9];
        for (int i = 0; i < 9; i++) {
            rotate_mat[i] = values[i];
        }
        do_rotation(rotate_mat);
    }
    else if (z_rotate != 0) {
        float values[9] = {cos(z_rotate * PI / UNIT), -sin(z_rotate * PI / UNIT), 0,
                   sin(z_rotate * PI / UNIT), cos(z_rotate * PI / UNIT), 0, 
                   0, 0, 1};

        float* rotate_mat = new float[9];
        for (int i = 0; i < 9; i++) {
            rotate_mat[i] = values[i];
        }
        do_rotation(rotate_mat);
    }

}


void Protein::set_shift(float shift_x, float shift_y, float shift_z) { 
    float* shift_mat = new float[3];
    shift_mat[0] = shift_x;
    shift_mat[1] = shift_y;
    shift_mat[2] = shift_z;
    do_shift(shift_mat);
}
void Protein::do_rotation(float * rotate_mat) {
    float avgx = 0;
    float avgy = 0;
    float avgz = 0;
    int num = 0;
    for (auto& [chainID, chain_atoms] : screen_atoms) {
        for (Atom& atom : chain_atoms) {
            avgx += atom.x;
            avgy += atom.y;
            avgz += atom.z;
            num += 1;   
        }
    }
    avgx /= num;
    avgy /= num;
    avgz /= num;
    for (auto& [chainID, chain_atoms] : screen_atoms) {
        for (Atom& atom : chain_atoms) {
            float x = atom.x;
            float y = atom.y;
            float z = atom.z;
            atom.x = avgx + rotate_mat[0] * (x - avgx) + rotate_mat[1] * (y - avgy) + rotate_mat[2] * (z - avgz);
            atom.y = rotate_mat[3] * (x - avgx) + avgy + rotate_mat[4] * (y - avgy) + rotate_mat[5] * (z - avgz);
            atom.z = rotate_mat[6] * (x - avgx) + rotate_mat[7] * (y - avgy) +  avgz + rotate_mat[8] * (z - avgz);
        }
    }
}

void Protein::do_shift(float* shift_mat) {
    for (auto& [chainID, chain_atoms] : screen_atoms) {
        for (Atom& atom : chain_atoms) {
            atom.x += shift_mat[0];
            atom.y += shift_mat[1];
            atom.z += shift_mat[2];
        }
    }
}