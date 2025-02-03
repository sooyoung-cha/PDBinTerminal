
#pragma once
#include <vector>
#include <cstring>

class Structure{
    private:
        vector <pair <int , int>> [2];

    public:
        Structure(int argc, char* argv[]);

        void print_args();

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
        int get_model(){
            return model;
        }
        int get_boxsize(){
            return boxsize;
        }
        bool get_show_structure(){
            return show_structure;
        }
        bool check_arg_okay(){
            return arg_okay;
        }
};