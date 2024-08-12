#include "Atom.hpp"

#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

class Protein {
public:
  Protein(int );
  void set_init_atoms(float *, int );
  void add_init_atoms(float , float , float );
  Atom* get_init_atoms();
  Atom* get_on_screen_atoms();
  void rotate(float , float , float ); // TDB: parameters
  void shift(float , float , float ); // TDB: parameters
  void zoom(float );  // TDB: parameters
private:
  Atom* m_init_atoms;
  Atom* m_on_screen_atoms;
  int m_last_atom_index;
  int m_length;
};

#endif