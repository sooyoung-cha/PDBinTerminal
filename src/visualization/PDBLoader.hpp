#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "../structure/Protein.hpp"


using namespace std;

class PDBLoader{
    // private:

    public:
        PDBLoader(){ return; }

        Protein* load_data(string in_file);
};
