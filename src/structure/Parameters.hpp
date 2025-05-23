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
        int width = 1;
        int height = 1;
        string in_file = "./test.pdb";
        string in_file2 = "none";
        string format = "pdb";
        string chains1 = "";
        string chains2 = "";
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
        string get_format(){
            return format;
        }
        string get_chains1(){
            return chains1;
        }
        string get_chains2(){
            return chains2;
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