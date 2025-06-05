#pragma once
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cctype>


using namespace std;

class Parameters{
    private:
        bool show_structure = false;
        bool arg_okay = true;
        int width = 3;
        int height = 3;
        string in_file = "none";
        string in_file2 = "none";
        string chains1 = "";
        string chains2 = "";
        string umatrix = "1,0,0,0,1,0,0,0,1";
        string tmatrix = "0,0,0";
        string mode = "default";

    public:
        Parameters(int argc, char* argv[]);
        bool issame = true;

        void print_args();

        bool is_valid_number(const std::string& str, int min, int max);

        // get, set
        string get_in_file1(){
            return in_file;
        }
        string get_in_file2(){
            return in_file2;
        }
        string get_chains1(){
            return chains1;
        }
        string get_chains2(){
            return chains2;
        }
        string get_umatrix(){
            return umatrix;
        }
        string get_tmatrix(){
            return tmatrix;
        }
        int get_width(){
            return width;
        }
        int get_height(){
            return height;
        }
        std::string get_mode(){
            return mode;
        }
        bool get_show_structure(){
            return show_structure;
        }
        bool check_arg_okay(){
            return arg_okay;
        }
};