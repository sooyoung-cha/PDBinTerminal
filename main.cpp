#include <iostream>
// #include "src/visualization/init.hpp"
// #include "src/visualization/loop.hpp"
#include "src/visualization/Screen.hpp"
#include "src/structure/Protein.hpp"
// #include "src/structure/Atom.hpp"

int main()
{
  // Atom a = Atom(0.5, 0.5, 3);
  Protein p = Protein(2);
  p.add_init_atoms(0.5, 0.5, 3);
  p.add_init_atoms(-0.5, 0.8, -2);

  Screen screen(80, 25);
  screen.set_protein(&p);
  screen.drawScreen();

  // init();
  // start();
  return 0;
}