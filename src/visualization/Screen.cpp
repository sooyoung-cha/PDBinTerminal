#include "Screen.hpp"
#include <ncurses.h>

#define NUM_PROTEIN 1

const float FOV = 90.0;
const float PI = 3.14159265359;

Screen::Screen(int width, int height) {
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    ASPECT_RATIO = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
    mScreen = new char[SCREEN_HEIGHT * SCREEN_WIDTH];
    clear_screen();
}

void Screen::set_protein(Protein* protein) {
  mProtein = protein;
}

void Screen::project() {
    float fovRad = 1.0 / tan(FOV * 0.5 / 180.0 * PI);
    const std::vector<Atom>& atoms = mProtein->get_on_screen_atoms();  

    clear_screen();

    for (const auto& atom : atoms) {
        float* position = atom.get_position();
        float x = position[0];
        float y = position[1];
        float z = position[2];
        float projectedX = (x / z) * fovRad * ASPECT_RATIO;
        float projectedY = (y / z) * fovRad;
        int screenX = (int)((projectedX + 1.0) * 0.5 * SCREEN_WIDTH);
        int screenY = (int)((1.0 - projectedY) * 0.5 * SCREEN_HEIGHT);

        if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT) {
            mScreen[screenY * SCREEN_WIDTH + screenX] = '*';
        }
    }
}

void Screen::clear_screen() {
    clear(); 
    std::fill(mScreen, mScreen + SCREEN_HEIGHT * SCREEN_WIDTH, '_');
}

void Screen::print_screen() {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            mvaddch(i, j, mScreen[i * SCREEN_WIDTH + j]); 
        }
    }
    refresh();
}

void Screen::drawScreen() {
    project();
    print_screen();
}

bool Screen::handle_input() {
    bool keep_show = true;

    switch (getch()) {
        case 'w':
        case 'W':
            mProtein->shift(0, 1);
            break;
        case 'a':
        case 'A':
            mProtein->shift(-1, 0);
            break;
        case 's':
        case 'S':
            mProtein->shift(0, -1);
            break;
        case 'd':
        case 'D':
            mProtein->shift(1, 0);
            break;
        case 'r':
        case 'R':
            mProtein->rotate(1, 0);
            break;
        case 'j':
        case 'J':
            mProtein->rotate(-1, 0);
            break;
        case 'i':
        case 'I':
            mProtein->rotate(0, 1);
            break;
        case 'k':
        case 'K':
            mProtein->rotate(0, -1);
            break;
        case 'z':
        case 'Z':
            mProtein->zoom(-1);
            break;
        case 'f':
        case 'F':
            mProtein->zoom(1);
            break;
        case 'q':
        case 'Q':
            keep_show = false;
            break;
        default:
            mvprintw(LINES - 1, 0, "Invalid input. Please try again."); 
            break;
    }

    return keep_show;
}