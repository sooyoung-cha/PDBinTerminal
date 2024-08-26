#include "../structure/Protein.hpp"
#include "../structure/Atom.hpp"
#include <vector>
#include <cmath>

#pragma once

class Screen {
public:
  Screen(int, int);
  void set_coords(Atom* );
  void set_protein(Protein* );
  void drawScreen();
private:
  int SCREEN_WIDTH;
  int SCREEN_HEIGHT;
  float ASPECT_RATIO;
  char* screen_chars;
  char* mScreen;
  Protein* mProtein;
  void project();
  void clear_screen();
};