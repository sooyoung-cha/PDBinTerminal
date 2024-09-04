#include "loop.hpp"

Manager::Manager(int argc, char* argv[]) : param(argc, argv) {
    init();
    loop();
}

void Manager::init(){
    if (param.check_arg_okay()){
        param.print_args();
        data = pdb_loader.load_data(param.get_in_file());
    }

    else{
        return;
    }
}

void Manager::loop(){
    bool keep_show = true;

    initscr(); 
    noecho(); 

    do{
        clear();
        // screen.show(data.get_on_screen_atoms);        
        refresh();                         
    }while(apply_user_input());

    endwin();
}

bool Manager::apply_user_input(){
            bool keep_show = true;
            switch(getch()){
                // W, w
                case 119:
                case 87:
                    data.shift(-1, 0, 0);
                    break;
                // A, a
                case 97:
                case 65:
                    data.shift(0, -1, 0);
                    break;
                // S, s
                case 115:
                case 83:
                    data.shift(1, 0, 0);
                    break;      
                // D, d
                case 100:
                case 68:
                    data.shift(0, 1, 0);
                    break;

                // Z, z
                case 122:
                case 90:
                    data.rotate(1, 0, 0);
                    break;  
                // C, c
                case 99:
                case 67:
                    data.rotate(-1, 0, 0);
                    break;   

                // R, R
                case 114:
                case 82:
                    data.zoom(-1);
                    break;   
                // F, f
                case 102:
                case 70:
                    data.zoom(1);
                    break;   

                // Q, q
                case 113:
                case 81:
                    keep_show = false;
                    break;

                default:
                    break;       
            }     
        }