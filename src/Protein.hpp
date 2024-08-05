#include "Atom.hpp"

#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

class Protein {
public:
  Protein();
  void set_init_atoms(float , float , float );
  void add_init_atoms(float , float , float );
  Atom* get_init_atoms();
  Atom* get_on_screen_atoms();
  void rotate(float , float , float ); // TDB: parameters
  void shift(float , float , float ); // TDB: parameters
  void zoome(float );  // TDB: parameters
private:
  Atom* init_atoms;
  Atom* on_screen_atoms;
};

#endif