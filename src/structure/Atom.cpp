#include "Atom.hpp"

Atom::Atom()
{
  mX = 0;
  mY = 0;
  mZ = 0;
}

Atom::Atom(float x, float y, float z)
{
  mX = x;
  mY = y;
  mZ = z;
}

float* Atom::get_position()
{
  float* coords = new float[3];

  coords[0] = mX;
  coords[1] = mY;
  coords[2] = mZ;

  return coords;
}

void Atom::set_position(float x, float y, float z)
{
  mX = x;
  mY = y;
  mZ = z;
}