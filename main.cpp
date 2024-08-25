#include <iostream>
#include "src/visualization/init.hpp"
#include "src/visualization/loop.hpp"

#ifdef OPENMP
#include <omp.h>
#endif

int main()
{
  init();
  start();
  return 0;
}