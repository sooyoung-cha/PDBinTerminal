#include "Protein.hpp"
#include "Atom.hpp"
#include <fstream>
#include <iostream>

Protein::Protein(const std::string& in_file) {
    load_data(in_file); 
    m_last_atom_index = m_init_atoms.size();  
}

void Protein::load_data(const std::string& in_file) {
    std::ifstream openFile(in_file);
    if (!openFile.is_open()) {
        std::cerr << "Error opening file: " << in_file << std::endl;
        return;
    }

    std::string line;
    while (getline(openFile, line)) {
        if (line.substr(0, 4) == "ATOM" && line.substr(13, 2) == "CA") {
            float x = std::stof(line.substr(30, 8));
            float y = std::stof(line.substr(38, 8));
            float z = std::stof(line.substr(46, 8));

            Atom new_atom;
            new_atom.mX = x;
            new_atom.mY = y;
            new_atom.mZ = z;

            m_init_atoms.push_back(new_atom);
            m_on_screen_atoms.push_back(new_atom);
        }
    }
    openFile.close();
}

int Protein::get_length() const {
    return m_init_atoms.size();
}

const std::vector<Atom>& Protein::get_init_atoms() const {
    return m_init_atoms;  
}

const std::vector<Atom>& Protein::get_on_screen_atoms() const {
    return m_on_screen_atoms;  // 화면에 표시할 Atom 벡터 반환
}

void Protein::zoom(float scale){
  zoom_level += scale;

  return;
}

void Protein::rotate(int right, int back) {
  float u[3][3];
  switch(right){
    case 1 :
      u[0][0] = 1;
      u[1][1] = 0.86602540378;
      u[1][2] = -0.5;
      u[2][1] = 0.5;
      u[2][2] = 0.86602540378;
      break;
    case -1 :
      u[0][0] = 1;
      u[1][1] = 0.86602540378;
      u[1][2] = 0.5;
      u[2][1] = -0.5;
      u[2][2] = 0.86602540378;
  }
  switch(back){
    case 1:
      u[0][0] = 0.86602540378;
      u[0][2] = 0.5;
      u[1][1] = 1;
      u[2][0] = -0.5;
      u[2][2] = 0.86602540378;
      break;
    case -1:
      u[0][0] = 0.86602540378;
      u[0][2] = -0.5;
      u[1][1] = 1;
      u[2][0] = 0.5;
      u[2][2] = 0.86602540378;
  }
  do_rotation(get_on_screen_atoms(), u);
}

void Protein::shift(int right, int up) { 
  float t[2];
  switch (right){
    case 1 :
    //TODO: should change the value here
      t[0] = -1;
      t[1] = 0;
      break;
    case -1:
      t[0] = 1;
      t[1] = 0;
  }
  switch (up){
    case 1 :
      t[0] = 0;
      t[1] = -1;
      break;
    case -1:
      t[0] = 0;
      t[1] = 1;
  }
  do_shift(get_on_screen_atoms(), t);
}

void Protein::print() {
  for (int i = 0; i < m_last_atom_index; i++) {
    m_on_screen_atoms[i].print_position();
  }
}