#include "Atom.hpp"
#include "../commons/simd.h"

#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

class Protein {
public:
  Protein(int );
  void set_init_atoms(float *, int );
  void add_init_atoms(float , float , float );
  Atom* get_init_atoms();
  // Protein* get_on_screen_atoms();
  int get_length();
  void rotate(int right, int back);
  void shift(int right, int up);
  void zoom(float scale); 
// private:
  Atom* m_init_atoms;
  Atom* m_on_screen_atoms;
  int m_last_atom_index;
  int m_length;
  float zoom_level;
};

static void do_rotation( Atom* & x, int len, float u[3][3]) {
    //len: length of atoms   int
    //u: rotation matrix     3x3

  simd_float u00 = simdf32_set(u[0][0]);
  simd_float u01 = simdf32_set(u[0][1]);
  simd_float u02 = simdf32_set(u[0][2]);
  simd_float u10 = simdf32_set(u[1][0]);
  simd_float u11 = simdf32_set(u[1][1]);
  simd_float u12 = simdf32_set(u[1][2]);
  simd_float u20 = simdf32_set(u[2][0]);
  simd_float u21 = simdf32_set(u[2][1]);
  simd_float u22 = simdf32_set(u[2][2]);
  for(int i=0; i<len; i+=VECSIZE_FLOAT) {
    simd_float x_x = simdf32_load(&x[i].mX);
    simd_float x_y = simdf32_load(&x[i].mY);
    simd_float x_z = simdf32_load(&x[i].mZ);
    simd_float xx = simdf32_mul(u00, x_x);
    simd_float yy = simdf32_mul(u01, x_y);
    simd_float zz = simdf32_mul(u02, x_z);
    xx = simdf32_add(xx, yy);
    simdf32_store(&x[i].mX, simdf32_add(xx, zz));
    xx = simdf32_mul(u10, x_x);
    yy = simdf32_mul(u11, x_y);
    zz = simdf32_mul(u12, x_z);
    xx = simdf32_add(xx, yy);
    simdf32_store(&x[i].mY,simdf32_add(xx, zz));
    xx = simdf32_mul(u20, x_x);
    yy = simdf32_mul(u21, x_y);
    zz = simdf32_mul(u22, x_z);
    xx = simdf32_add(xx, yy);
    simdf32_store(&x[i].mZ, simdf32_add(xx, zz));
  }
}

static void do_shift(Atom* & x, int len, float t[2]) {
  for(int i=0; i<len; i+=VECSIZE_FLOAT) {
    simd_float t0 = simdf32_set(t[0]);
    simd_float t1 = simdf32_set(t[1]);
    simd_float x_x = simdf32_load(&x[i].mX);
    simd_float x_y = simdf32_load(&x[i].mY);
    simdf32_store(&x[i].mX, simdf32_add(x_x, t0));
    simdf32_store(&x[i].mY, simdf32_add(x_y, t1));
  }
}
#endif