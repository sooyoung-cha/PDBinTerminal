#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct Atom {
    int pos;
    double x, y, z;
};

std::vector<Atom> readPDB(const std::string& filename) {
    std::vector<Atom> calphaAtoms;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.compare(0, 4, "ATOM") == 0 && line.size() > 54) {
            if (line.compare(12, 4, " CA ") == 0) {
                double x = std::stod(line.substr(30, 8));
                double y = std::stod(line.substr(38, 8));
                double z = std::stod(line.substr(46, 8));
                int pos = stoi(line.substr(22, 4));
                calphaAtoms.push_back({x, y, z});
            }
        }
    }
    return calphaAtoms;
}

int main() {
    std::string filename = "example.pdb";
    std::vector<Atom> calphaAtoms = readPDB(filename);
    return 0;
}
