#include "PDBLoader.hpp"

Protein load_data(string in_file){
    Protein data;
    ifstream openFile(in_file.data()); 

    if(openFile.is_open()){
        string line;
        while(getline(openFile, line)){
            if(line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA"){
                data.add_init_atoms(stof(line.substr(30, 8)), stof(line.substr(38, 8)), stof(line.substr(46, 8)));
            }
        }
    }               
    return data;
}