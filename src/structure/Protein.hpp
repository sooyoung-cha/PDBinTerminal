#include "Atom.hpp"
#include "../commons/simd.h"
#include <vector>

#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

class Protein {
public:
  Protein(const std::string& in_file, const std::string& chains, const bool& show_structure); 
  std::unordered_map<char, std::unordered_map<int, Atom>>& get_init_atoms();
  std::unordered_map<char, std::unordered_map<int, Atom>>& get_on_screen_atoms();

  int get_length();
  void set_shift(int x_move, int y_move, int z_move);
  void set_rotate(int x_rotate, int y_rotate, int z_rotate);

private:
  std::unordered_map<char, std::unordered_map<int, Atom>> init_atoms; 
  std::unordered_map<char, std::unordered_map<int, Atom>> on_screen_atoms; 

  void load_data(const std::string& in_file, const std::string& chains, const bool& show_structure);
  void load_ca(const std::string& in_file, const std::string& chains);
  void load_structure(const std::string& in_file, const std::string& chains);
  
  void do_rotation(float u[3][3]);
  void do_shift(float t[3]);
};


#endif