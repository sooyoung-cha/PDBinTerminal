#include "Screen.hpp"
#include <iostream>

#define NUM_PROTEIN 1

const float FOV = 90.0;
const float PI = 3.14159265359;

// class Screen {
// public:
//   void set_coords(Atom* );
// private:
//   char* screen_chars;
//   Protein* on_screen_protein;
//   void atoms_to_screen_plane();
//   void screen_plane_visualization();
// };

void Screen::set_protein(Protein* protein)
{
  mProtein = protein;
}

Screen::Screen(int width, int height)
{
  SCREEN_WIDTH = width;
  SCREEN_HEIGHT  = height;
  ASPECT_RATIO = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
  // std::vector<std::vector<char> > mScreen(SCREEN_HEIGHT, std::vector<char>(SCREEN_WIDTH, '_'));
  // mScreen = std::vector<std::vector<char>>(SCREEN_HEIGHT, std::vector<char>(SCREEN_WIDTH, '_'));
  mScreen = new char[SCREEN_HEIGHT*SCREEN_WIDTH];
  for (int i = 0; i < SCREEN_HEIGHT; i++)
  {
    for (int j = 0; j < SCREEN_WIDTH; j++)
    {
      mScreen[i*SCREEN_WIDTH + j] = '_';
    }
  }

  // std::cout << mScreen[0] << std::endl;
}

void Screen::project() {
  float fovRad = 1.0 / tan(FOV * 0.5 / 180.0 * PI);

  int L = (*mProtein).get_length();
  Atom* atoms = (*mProtein).get_init_atoms();

  std::cout << "?" << std::endl;
  
  for (int l = 0; l < L; l++)
  {
    // atoms[l].print_position();
    float* atom = atoms[l].get_position();
    float x = atom[0];
    float y = atom[1];
    float z = atom[2];
    float projectedX = (x / z) * fovRad * ASPECT_RATIO;
    float projectedY = (y / z) * fovRad;
    int screenX = (int)((projectedX + 1.0) * 0.5 * SCREEN_WIDTH);
    int screenY = (int)((1.0 - projectedY) * 0.5 * SCREEN_HEIGHT);

    if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT) {
      mScreen[screenY * SCREEN_WIDTH + screenX] = '*';
    }
    // std::cout << screenY << " " << screenX << std::endl;
  }
  // std::cout << "!" << std::endl;
}

void Screen::clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}

void Screen::drawScreen() {
    Screen::clear_screen();
    Screen::project();

    std::cout << "project done" << std::endl;

    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
      for (int j = 0; j < SCREEN_WIDTH; j++)
      {
        std::cout << mScreen[i*SCREEN_WIDTH + j];
      }
      std::cout << "\n";
    }
    // std::cout << 'hi' << std::endl;
}