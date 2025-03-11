#pragma once
#include <iostream>
#include <cstring>

using namespace std;

class Parameters{
    private:
        string in_file = "./test.pdb";
        string format = "pdb";
        string chains = "";
        int model = 1;
        int width = 1;
        int height = 1;
        bool show_structure = false;
        bool arg_okay = true;

    public:
        Parameters(int argc, char* argv[]);

        void print_args();

        bool is_valid_number(const std::string& str, int min, int max);

        // get, set
        string get_in_file(){
            return in_file;
        }
        string get_format(){
            return format;
        }
        string get_chains(){
            return chains;
        }
        int get_width(){
            return width;
        }
        int get_height(){
            return height;
        }
        bool get_show_structure(){
            return show_structure;
        }
        bool check_arg_okay(){
            return arg_okay;
        }
};