#pragma once
#include <iostream>
#include <cstring>

using namespace std;

class Parameters{
    // private:
        string in_file = "./test.pdb";
        string format = "pdb";
        string chains = "";
        int model = 1;
        int boxsize = 20;
        bool arg_okay = true;

    public:
        Parameters(int argc, char* argv[]);

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
        bool check_arg_okay(){
            return arg_okay;
        }
};