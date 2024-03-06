class PDBViewer{
    private:
    int x_limit = 60;
    int y_limit = 60;

    public:
        PDBViewer(){
            return;
        }

        bool check_move(int dots[][3], int num_dots, int new_x_move, int new_y_move){
            for(int i = 0; i < num_dots; i++) {
                if(dots[i][x] + new_x_move < 0) return false;
                else if(dots[i][y] + new_y_move < 0) return false;
            }
            return true;
        }

        void print_atoms(int dots[][3], int num_dots){
            char user_input;
            bool keep_show = true;
            int x_move, y_move;
            x_move = 0;
            y_move = 0;

            initscr(); 
            noecho(); 

            do{
                clear();
                for(int i = 0; i < num_dots; i++) {
                    move(dots[i][x] + x_move, dots[i][y] + y_move);
                    printw("*"); 
                }
                refresh();

                switch(getch()){
                    // A, a
                    case 97:
                    case 65:
                        if(check_move(dots, num_dots, x_move, y_move - 1)){
                            y_move--;
                        }
                        break;
                    // W, w
                    case 119:
                    case 87:
                        if(check_move(dots, num_dots, x_move - 1, y_move)){
                            x_move--;
                        }
                        break;
                    // D, d
                    case 100:
                    case 68:
                        if(check_move(dots, num_dots, x_move, y_move + 1)){
                            y_move++;
                        }
                        break;
                    // S, s
                    case 115:
                    case 83:
                        if(check_move(dots, num_dots, x_move + 1, y_move)){
                            x_move++;
                        }
                        break;       
                    default:
                        keep_show = false;
                        break;             
                } 
            }while(keep_show);

            endwin();
        }
};