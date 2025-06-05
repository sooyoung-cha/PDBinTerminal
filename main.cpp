#include <iostream>
#include <ncurses.h>
#include <unistd.h> 
#include "src/structure/Protein.hpp"
#include "src/structure/Parameters.hpp"
#include "src/visualization/Screen.hpp"

int main(int argc, char* argv[]) {
    Parameters params(argc, argv);

    if (!params.check_arg_okay()) {
        return -1; 
    }
    params.print_args();

    initscr(); // ncurses 초기화
    cbreak();  // 특수 키 입력을 받도록 설정
    noecho();  // 입력한 문자를 화면에 표시하지 않음

    Screen screen(params.get_width(), params.get_height(), params.get_show_structure(), params.get_mode(), params.issame); 

    Protein protein1(params.get_in_file1(), params.get_chains1(), params.get_show_structure());    
    screen.set_protein1(&protein1);
    
    Protein protein2(params.get_in_file2(), params.get_chains2(), params.get_show_structure());
    screen.set_protein2(&protein2, params.get_umatrix(), params.get_tmatrix());

    bool run = true;
    while(run) {
        screen.drawScreen();
        run = screen.handle_input();
        usleep(100);
    }

    endwin(); // ncurses 종료
    return 0;
}