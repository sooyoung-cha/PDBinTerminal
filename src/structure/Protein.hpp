#include "Atom.hpp"
#include "../commons/simd.h"
#include <vector>
#include <map>

#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

class Protein {
public:
  Protein(const std::string& in_file, const std::string& chains, const bool& show_structure); 
  ~Protein();
  std::map<char, Atom*>& get_init_atoms();
  std::map<char, Atom*>& get_on_screen_atoms();
  std::map<char, int>& get_num_chain_Atoms();
  int& get_num_chain_Atoms(char chainID);

  int get_length();
  void set_shift(float shift_x, float shift_y, float shift_z);
  void set_rotate(int x_rotate, int y_rotate, int z_rotate);

private:
  std::map<char, Atom*> init_atoms; 
  std::map<char, Atom*> on_screen_atoms; 

  std::map<char, int> num_chain_Atoms;

  void load_data(const std::string& in_file, const std::string& target_chains, const bool& show_structure);
  void get_cnt_atom_ss(const std::string& in_file, const std::string& target_chains, 
                       std::vector<std::tuple<char, int, char, int, char>>& sec_struct_info,
                       bool show_structure);
  void load_ca(const std::string& in_file, const std::string& target_chains,
               std::vector<std::tuple<char, int, char, int, char>> sec_struct_info,
               bool show_structure);
  
  void do_rotation(float u[3][3]);
  void do_shift(float t[3]);
};


#endif