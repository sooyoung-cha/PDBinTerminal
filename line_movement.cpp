#include <ncurses.h>
#include <algorithm> // For std::min

class Line {
public:
    int x, y, length;
    bool horizontal; // True if horizontal, false if vertical

    Line(int fieldWidth) : x(1), y(1), horizontal(true) {
        length = std::min(5, fieldWidth / 20); // Ensure the line is shorter than 1/20 of the field width, example length
    }

    void move(int ch, int maxWidth, int maxHeight) {
        switch (ch) {
            case 'w': if (y > 1) --y; break;
            case 's': if (y < maxHeight - 2) ++y; break;
            case 'a': if (x > 1) x = std::max(1, x - 1); break;
            case 'd': if (x < maxWidth - 2 - length) x = std::min(maxWidth - 2 - length, x + 1); break;
        }
    }
};

class Field {
private:
    int width, height;
    Line line;

public:
    Field(int w, int h) : width(w), height(h), line(w) {}

    void update(int ch) {
        line.move(ch, width, height);
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
        for (int l = 0; l < line.length; ++l) {
            mvaddch(line.y, line.x + l, '*'); // Draw horizontal line
        }
        refresh();
    }
};

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int ch;
    Field field(100, 25);

    while((ch = getch()) != 'q') {
        field.update(ch);
        field.display();
    }

    endwin();

    return 0;
}

