#include <ncurses.h>

const int MAX_WIDTH = 80;
const int MAX_HEIGHT = 24;
char screen[MAX_HEIGHT][MAX_WIDTH + 1]; // +1 for the null terminator

// Draw the rod on the screen buffer
void drawRod(int x, int y, int length, bool horizontal) {
    // Clear the screen buffer
    for (int i = 0; i < MAX_HEIGHT; ++i)
        for (int j = 0; j < MAX_WIDTH; ++j)
            screen[i][j] = ' ';

    // Draw the rod based on its orientation
    if (horizontal) {
        for (int i = x - length / 2; i <= x + length / 2 && i < MAX_WIDTH && i >= 0; ++i)
            if (y >= 0 && y < MAX_HEIGHT) screen[y][i] = '#';
    } else {
        for (int i = y - length / 2; i <= y + length / 2 && i < MAX_HEIGHT && i >= 0; ++i)
            if (x >= 0 && x < MAX_WIDTH) screen[i][x] = '#';
    }

    // Null-terminate each row for printw
    for (int i = 0; i < MAX_HEIGHT; ++i) screen[i][MAX_WIDTH] = '\0';
}

int main() {
    initscr();            // Initialize ncurses
    cbreak();             // Disable line buffering
    keypad(stdscr, TRUE); // Enable function keys
    noecho();             // Disable echo

    int x = MAX_WIDTH / 2, y = MAX_HEIGHT / 2;
    int length = 10; // Length of the rod
    bool horizontal = true; // Orientation of the rod

    while (true) {
        clear(); // Clear the screen
        drawRod(x, y, length, horizontal); // Draw the rod

        // Print the screen buffer
        for (int i = 0; i < MAX_HEIGHT; ++i)
            printw("%s\n", screen[i]);

        refresh(); // Refresh the screen to show the drawing

        int ch = getch(); // Get a character from the user

        // Handle input
        switch (ch) {
            case 'a': x--; break; // Move left
            case 'd': x++; break; // Move right
            case 'w': y--; break; // Move up
            case 's': y++; break; // Move down
            case 'p': horizontal = !horizontal; break; // Rotate right
            case 'o': horizontal = !horizontal; break; // Rotate left
            case 'x': endwin(); return 0; // Exit the program
        }
    }

    endwin(); // End ncurses mode
    return 0;
}

