#pragma once
#include "Protein.hpp"
#include "Parameters.hpp"
#include "PDBLoader.hpp"
#include "Screen.hpp"


#include <ncurses.h>

#include <typeinfo>

// get keyboard input and throw it to Protein instance
void start(Protein data)
{
  // show the list of coordinates
  // get keyboard
  // show the list of the modified coordinates
}

class Manager{
private:
  Parameters param;
  PDBLoader pdb_loader;
  Protein* data;
  Screen screen;

public:
  Manager(int argc, char* argv[]);
  ~Manager();

  void init();

  void loop();

  bool apply_user_input();
};
