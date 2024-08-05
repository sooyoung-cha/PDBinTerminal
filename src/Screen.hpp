#include "ProteinData.hpp"

#pragma once

class Screen {
public:
  void set_coords(Atom* );
private:
  char* screen_chars;
  Protein* on_screen_protein;
  void atoms_to_screen_plane();
  void screen_plane_visualization();
};