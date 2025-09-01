#include "Atom.hpp"
#include <map>
#include <cmath>
#include <vector>
#include <cstdlib>

#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

class StructureMaker {
public:
    StructureMaker(); 
    ~StructureMaker();

    void calculate_ss_points(std::map<char, std::vector<Atom>>& init_atoms,
                           std::map<char, std::vector<Atom>>& ss_atoms);
    void computeHelixAxis(const std::vector<Atom>& helix, float (&center)[3], float (&axis)[3]);
    std::vector<std::vector<Atom>> extractHelixSegments(const Atom* atoms, int num_atoms);
private:
    float radius = 0.08f;
    int circle_steps = 8; 
    int width = 2;
};


#endif