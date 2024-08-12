#include "loop.hpp"
#include <iostream>
#include "Atom.hpp"

using namespace std;

// get keyboard input and throw it to Protein instance
void start(Protein data)
{
  float* coords = new float[3];

  while (true)
  {
    for (int i = 0; data.get_length(); i++)
    {
      Atom* atom = data.get_init_atoms();

      coords = atom->get_position();

      cout << coords[0] << " " << coords[1] << " " << coords[2] << endl;
    }

    // get keyboard
    // Todo: sungeun

    // wasd: shift
    // m: zoom in
    // n: zoom out
    // hkuj: rotate
  }
}