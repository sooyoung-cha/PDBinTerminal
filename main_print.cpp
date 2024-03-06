#include "lib.h"
#include "data.h"
#include "parameter.h"
#include "test_viewer.h"

// g++ -std=c++17 main_print.cpp -o main_print.o -lncurses

int main(int argc, char *argv[]){
    PDBViewer viewer;
    int dots[5][3] = {{10, 10, 0}, {9, 10, 0}, {10, 9, 0}, {11, 10, 0}, {10, 11, 0}};
    int num_dots = 5;

    viewer.print_atoms(dots, num_dots);

    return 0;    
}