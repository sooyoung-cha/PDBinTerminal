#include "Atom.hpp"
#include "../commons/simd.h"
#include <vector>

#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP

class Protein {
public:
  Protein(const std::string& in_file); 
  const std::vector<Atom>& get_init_atoms() const; 
  const std::vector<Atom>& get_on_screen_atoms() const;
  int get_length() const;

  void rotate(int right, int back);
  void shift(int right, int up);
  void zoom(float scale); 

private:
  std::vector<Atom> m_init_atoms; 
  std::vector<Atom> m_on_screen_atoms; 
  int m_last_atom_index;
  float zoom_level;

  void load_data(const std::string& in_file);  // PDB 파일을 읽는 함수
};

static void do_rotation(const std::vector<Atom>& atoms, float u[3][3]) {
    int len = atoms.size();
    simd_float u00 = simdf32_set(u[0][0]);
    simd_float u01 = simdf32_set(u[0][1]);
    simd_float u02 = simdf32_set(u[0][2]);
    simd_float u10 = simdf32_set(u[1][0]);
    simd_float u11 = simdf32_set(u[1][1]);
    simd_float u12 = simdf32_set(u[1][2]);
    simd_float u20 = simdf32_set(u[2][0]);
    simd_float u21 = simdf32_set(u[2][1]);
    simd_float u22 = simdf32_set(u[2][2]);

    for (int i = 0; i < len; i += VECSIZE_FLOAT) {
        simd_float x_x = simdf32_load(&atoms[i].mX);
        simd_float x_y = simdf32_load(&atoms[i].mY);
        simd_float x_z = simdf32_load(&atoms[i].mZ);
        
        simd_float xx = simdf32_mul(u00, x_x);
        simd_float yy = simdf32_mul(u01, x_y);
        simd_float zz = simdf32_mul(u02, x_z);
        xx = simdf32_add(xx, yy);
        simdf32_store(&atoms[i].mX, simdf32_add(xx, zz));
        
        xx = simdf32_mul(u10, x_x);
        yy = simdf32_mul(u11, x_y);
        zz = simdf32_mul(u12, x_z);
        xx = simdf32_add(xx, yy);
        simdf32_store(&atoms[i].mY, simdf32_add(xx, zz));
        
        xx = simdf32_mul(u20, x_x);
        yy = simdf32_mul(u21, x_y);
        zz = simdf32_mul(u22, x_z);
        xx = simdf32_add(xx, yy);
        simdf32_store(&atoms[i].mZ, simdf32_add(xx, zz));
    }
}

static void do_shift(const std::vector<Atom>& atoms, float t[2]) {
    int len = atoms.size();
    for (int i = 0; i < len; i += VECSIZE_FLOAT) {
        simd_float t0 = simdf32_set(t[0]);
        simd_float t1 = simdf32_set(t[1]);
        simd_float x_x = simdf32_load(&atoms[i].mX);
        simd_float x_y = simdf32_load(&atoms[i].mY);
        
        simdf32_store(&atoms[i].mX, simdf32_add(x_x, t0));
        simdf32_store(&atoms[i].mY, simdf32_add(x_y, t1));
    }
}

#endif