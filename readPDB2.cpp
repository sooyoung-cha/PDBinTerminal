#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<double> readPDB(const std::string& filename, int& len) {
    std::vector<double> qdata;
    std::ifstream file(filename);
    std::string line;

    std::vector<double> tempX, tempY, tempZ;

    while (std::getline(file, line)) {
        if (line.compare(0, 4, "ATOM") == 0 && line.size() > 54) {
            if (line.compare(12, 4, " CA ") == 0) {
                double x = std::stod(line.substr(30, 8));
                double y = std::stod(line.substr(38, 8));
                double z = std::stod(line.substr(46, 8));
                tempX.push_back(x);
                tempY.push_back(y);
                tempZ.push_back(z);
                len++;
            }
        }
    }
    qdata.resize(3 * len);
    for (int i = 0; i < len; i++) {
        qdata[i] = tempX[i];
        qdata[len + i] = tempY[i];
        qdata[2 * len + i] = tempZ[i];
    }
    return qdata;
}

int main() {
    std::string filename = "example.pdb";
    int len = 0;
    std::vector<double> qdata = readPDB(filename, len);
    return 0;
}
