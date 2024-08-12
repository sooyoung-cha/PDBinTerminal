#include "init.hpp"

// initiate instances, Load the PDB file to Protein instance
// result: instances, PDB loaded Protein instance
Protein init(int argc, char* argv[]){    
    Parameters param(argc, argv);
    PDBLoader Loader;

    if (param.check_arg_okay()){
        param.print_args();
        return Loader.load_data(param.get_in_file());
    }

    else{
        return;
    }
}