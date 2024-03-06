#include "lib.h"
#include "data.h"
#include "parameter.h"
#include "viewer.h"

int main(int argc, char *argv[]){
    cout << "RUN PDBinTerminal" << endl;
    Parameters param(argc, argv);
    PDBViewer viewer;
    PDBReader reader;

    if (param.check_arg_okay()){
        param.print_args();
        reader.read_file(param);
        PDBData pdb_data = reader.get_data();
        
        /*
        viewer.print_data(param, pdb_data);
        */
    }

    return 0;    
}