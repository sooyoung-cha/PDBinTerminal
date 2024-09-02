#include "PDBLoader.hpp"

Protein load_data(string in_file){
    int len = 0;

    ifstream openFile_len(in_file.data()); 
    if(openFile.is_open()){
        string line;
        while(getline(openFile, line)){
            if(line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA"){
                len += 1;
            }
        }
    }     
    openFile_len.close();

    Protein data(len);
    ifstream openFile_data(in_file.data()); 
    if(openFile.is_open()){
        string line;
        while(getline(openFile, line)){
            if(line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA"){
                data.add_init_atoms(stof(line.substr(30, 8)), stof(line.substr(38, 8)), stof(line.substr(46, 8)));
            }
        }
    }      
    openFile_data.close();

    return data;
}