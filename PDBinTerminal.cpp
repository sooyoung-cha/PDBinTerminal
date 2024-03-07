#include <iostream>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>

struct Point {
    int colPos;
    int linePos;
    int columns;
    int lines;
    WINDOW* win;

    Point() {};
    Point(WINDOW* win, int columns, int lines) : columns(columns), lines(lines), win(win) {
        srand(static_cast<unsigned int>(time(nullptr)));
        colPos = rand() % columns;
        linePos = rand() % lines;
        show();
    };
    void show(){
        if (win != nullptr){
            mvwaddch(win, linePos,colPos, '.');
            wrefresh(win);
        }
    }

    void move(char key){
        switch(key){
            case 'w':
                linePos -= 1;
                if (linePos < 0){
                    linePos = lines -1;
                }
                break;
            case 'a':
                colPos -= 1;
                if (colPos < 0){
                    colPos = columns -1;
                }
                break;
            case 's':
                linePos += 1;
                if (linePos >= lines){
                    linePos = 0;
                }
                break;
            case 'd':
                colPos += 1;
                if (colPos >= columns){
                    colPos = 0;
                }
                break;
            default :
                break;
        }
    }
    
};

int main(){
    
    int columns, lines;

    initscr();
    getmaxyx(stdscr, lines, columns);
    keypad(stdscr, TRUE);
    curs_set(0);

    WINDOW* win = newwin(lines, columns, 0, 0);
    wclear(win);
    box(win, '|', '-');
    wrefresh(win);


    char keyboard;
    keyboard = (char) getch();

    Point point(win, columns, lines);
    wclear(win);
    box(win, '|', '-');
    wrefresh(win);
    point.show();

    if (keyboard != 'q'){
        do {
            point.move(keyboard);
            wclear(win);
            box(win, '|', '-');
            point.show();
        
            keyboard = (char) getch();
        } while (keyboard != 'q');
    }
    endwin();
    
    return 0;
}