#include <ncurses.h>

class Dot {
public:
    int x, y;

    Dot() : x(1), y(1) {} // Start inside the margins

    void move(int ch, int maxWidth, int maxHeight) {
        switch (ch) {
            case 'w': if (y > 1) --y; break;
            case 's': if (y < maxHeight - 2) ++y; break;
            case 'a': if (x > 1) --x; break;
            case 'd': if (x < maxWidth - 2) ++x; break;
        }
    }
};

class Field {
private:
    int width, height;
    Dot dot;

public:
    Field(int w, int h) : width(w), height(h), dot() {}

    void update(int ch) {
        dot.move(ch, width, height);
    }

    void display() {
        clear();
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (i == 0 || i == height - 1) mvaddch(i, j, '-');
                else if (j == 0 || j == width - 1) mvaddch(i, j, '|');
                else mvaddch(i, j, ' ');
            }
        }
        mvaddch(dot.y, dot.x, '*');
        refresh();
    }
};

int main() {
    initscr(); // Start curses mode
    cbreak(); // Line buffering disabled
    noecho(); // Don't echo while we do getch
    keypad(stdscr, TRUE); // Get special keys like arrows
    curs_set(0); // Don't display the cursor

    int ch;
    Field field(100, 25);

    while((ch = getch()) != 'q') {
        field.update(ch);
        field.display();
    }

    endwin(); // End curses mode

    return 0;
}

