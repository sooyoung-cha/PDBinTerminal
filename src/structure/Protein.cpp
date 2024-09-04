#include "Protein.hpp"
#include "Atom.hpp"

Protein::Protein(int length)
{
    m_init_atoms = new Atom[length];
    m_length = length;
    m_last_atom_index = 0;
}

void Protein::set_init_atoms(float* coords, int len)
{
  for (int i = 0; i < len; i++)
  {
    add_init_atoms(coords[0], coords[1], coords[2]);
  }
}

int Protein::get_length(){
  return m_length;
}

void Protein::add_init_atoms(float x, float y, float z)
{
  if (m_last_atom_index >= m_length)
  {
    return;
  }
  m_init_atoms[m_last_atom_index].set_position(x, y, z);
  m_last_atom_index++;
}


Atom* Protein::get_init_atoms()
{
  Atom* atoms = new Atom[m_last_atom_index];
  float* coords = new float[3];

  for (int i = 0; i < m_last_atom_index; i++)
  {
    coords = m_init_atoms[i].get_position();
    atoms[i].set_position(coords[0], coords[1], coords[2]);
  }

  return atoms;
}

void Protein::zoom(float scale){
  zoom_level += scale;

  return;
}

void Protein::rotate(int right, int back){
  float u[3][3];
  switch(right){
    case 1 :
      u[0][0] = 1;
      u[1][1] = 0.86602540378;
      u[1][2] = -0.5;
      u[2][1] = 0.5;
      u[2][2] = 0.86602540378;
      break;
    case -1 :
      u[0][0] = 1;
      u[1][1] = 0.86602540378;
      u[1][2] = 0.5;
      u[2][1] = -0.5;
      u[2][2] = 0.86602540378;
  }
  switch(back){
    case 1:
      u[0][0] = 0.86602540378;
      u[0][2] = 0.5;
      u[1][1] = 1;
      u[2][0] = -0.5;
      u[2][2] = 0.86602540378;
      break;
    case -1:
      u[0][0] = 0.86602540378;
      u[0][2] = -0.5;
      u[1][1] = 1;
      u[2][0] = 0.5;
      u[2][2] = 0.86602540378;
  }
  do_rotation(m_init_atoms, m_length, u);
  return;
}