#include <iostream>
#include <ncurses.h>

int main()
{
  initscr();
  cbreak(); // Disable line buffering
  noecho(); // Don't echo input to the screen
  keypad(stdscr, TRUE); // Enable keypad mode

  int ci = 0, cj = 0;

  char key_input;
  // Screen myScreen = Screen(0, 0);
  
  std::cout << "test" << std::endl;

  while (1) {
    key_input = getch();
    clear();
    // std::cout << key_input << std::endl;

    switch (key_input) {
      case 'w':
        ci -= 1;
        break;
      case 'a':
        cj -= 1;
        break;
      case 's':
        ci += 1;
        break;
      case 'd':
        cj += 1;
        break;
    }

    if (key_input == 'q') {
      break;
    }
    mvprintw(ci, cj, "O");
    refresh(); // Refresh the screen
  }

  endwin(); // End ncurses mode
  return 0;
}