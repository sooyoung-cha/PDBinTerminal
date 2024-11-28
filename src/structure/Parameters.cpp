#include "Parameters.hpp"

Parameters::Parameters(int argc, char* argv[]) {
    // get in_file
    if (argc > 1) {
        in_file = argv[1];
    }
    else {
        cout << "Need input file dir !!!" << endl;
        arg_okay = false;
    }

    // get format, chains, model, boxsize
    for (int i = 2; i < argc; i++) {
        try{
            if (!strcmp(argv[i],"-f") || !strcmp(argv[i],"--format")) {
                if (!strcmp(argv[i+1], "pdb") || !strcmp(argv[i+1], "mmcif") || !strcmp(argv[i+1], "mmtf") || !strcmp(argv[i+1], "mae") || !strcmp(argv[i+1], "maegz")){
                    format = argv[i+1];
                    i++;
                }
                else {throw;}
            }
            else if (!strcmp(argv[i],"-m") || !strcmp(argv[i],"--model")) {
                model = atoi(argv[i+1]);
                i++;
            }
            else if (!strcmp(argv[i],"-c") || !strcmp(argv[i],"--chains")) {
                chains = argv[i+1];
                i++;
            }
            else if (!strcmp(argv[i],"-s") || !strcmp(argv[i],"--boxsize")) {
                boxsize = atoi(argv[i+1]);
                i++;
            }
            else { 
                throw;
            }
        }
        catch(...) {
            cout << "Wrong input parameters !!!" << endl;
            cout << argv[i] << endl;
            arg_okay = false;
        }
    }
}

void Parameters::print_args() {
    cout << "Input parameters >> " << endl;
    cout << "  in_file: " << in_file << endl;
    cout << "  format: " << format << endl;
    cout << "  model: " << model << endl;
    cout << "  chains: " << chains << endl;
    cout << "  boxsize: " << boxsize << endl;
    return;
}