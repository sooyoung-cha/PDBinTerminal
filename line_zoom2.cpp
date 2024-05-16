#include <ncurses.h>

const int MAX_WIDTH = 80;
const int MAX_HEIGHT = 24;
char screen[MAX_HEIGHT][MAX_WIDTH + 1]; // +1 for the null terminator

enum Orientation {
    Horizontal,
    Vertical,
    DiagonalUp,
    DiagonalDown
};

void drawRod(int x, int y, int length, Orientation orientation) {
    // Clear the screen buffer
    for (int i = 0; i < MAX_HEIGHT; ++i)
        for (int j = 0; j < MAX_WIDTH; ++j)
            screen[i][j] = ' ';

    switch (orientation) {
        case Horizontal:
            for (int i = x - length / 2; i <= x + length / 2 && i < MAX_WIDTH && i >= 0; ++i)
                if (y >= 0 && y < MAX_HEIGHT) screen[y][i] = '_';
            break;
        case Vertical:
            for (int i = y - length / 2; i <= y + length / 2 && i < MAX_HEIGHT && i >= 0; ++i)
                if (x >= 0 && x < MAX_WIDTH) screen[i][x] = '|';
            break;
        case DiagonalDown:
            for (int i = -length / 2; i <= length / 2 && x + i < MAX_WIDTH && x + i >= 0 && y + i < MAX_HEIGHT && y + i >= 0; ++i)
                screen[y + i][x + i] = '\\';
            break;
        case DiagonalUp:
            for (int i = -length / 2; i <= length / 2 && x + i < MAX_WIDTH && x + i >= 0 && y - i < MAX_HEIGHT && y - i >= 0; ++i)
                screen[y - i][x + i] = '/';
            break;
    }

    // Null-terminate each row for printw
    for (int i = 0; i < MAX_HEIGHT; ++i) screen[i][MAX_WIDTH] = '\0';
}

Orientation getNextOrientation(bool clockwise, Orientation current) {
    if (clockwise) {
        switch (current) {
            case Horizontal: return DiagonalDown;
            case DiagonalDown: return Vertical;
            case Vertical: return DiagonalUp;
            case DiagonalUp: return Horizontal;
        }
    } else { // Counterclockwise
        switch (current) {
            case Horizontal: return DiagonalUp;
            case DiagonalUp: return Vertical;
            case Vertical: return DiagonalDown;
            case DiagonalDown: return Horizontal;
        }
    }
    return Horizontal; // Default case to prevent compiler warning
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int x = MAX_WIDTH / 2, y = MAX_HEIGHT / 2;
    int length = 10;
    Orientation orientation = Horizontal;

    while (true) {
        clear();
        drawRod(x, y, length, orientation);

        for (int i = 0; i < MAX_HEIGHT; ++i)
            printw("%s\n", screen[i]);
            
        refresh();

        int ch = getch();

        switch (ch) {
            case 'a': x--; break;
            case 'd': x++; break;
            case 'w': y--; break;
            case 's': y++; break;
            case 'z': 
                if (length < MAX_WIDTH - 5) {
                    length += 5; // Zoom in, ensuring the length does not exceed screen width
                }
                break;
            case 'c': 
                if (length > 5) {
                    length -= 5; // Zoom out, ensuring the length does not go below minimum
                }
                break;
            case 'p': orientation = getNextOrientation(true, orientation); break;
            case 'o': orientation = getNextOrientation(false, orientation); break;
            case 'x': endwin(); return 0; // Exit the program
        }
    }

    endwin();
    return 0;
}

