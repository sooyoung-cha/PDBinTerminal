#include <iostream>
// #include "src/visualization/init.hpp"
#include "src/visualization/loop.hpp"
#include "src/visualization/Screen.hpp"
#include "src/structure/Protein.hpp"
// #include "src/structure/Atom.hpp"

#ifdef OPENMP
#include <omp.h>
#endif

int main(int argc, char* argv[])
{
  Manager manager(argc, argv);
  return 0;
}