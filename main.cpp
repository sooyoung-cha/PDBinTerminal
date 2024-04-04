#include "lib.h"
#include "data.h"
#include "parameter.h"
#include "viewer.h"

int main(int argc, char *argv[]){
    cout << "RUN PDBinTerminal" << endl;
    Parameters param(argc, argv);
    
    PDBReader reader;

    if (param.check_arg_okay()){
        param.print_args();
        reader.read_file(param);
        PDBData pdb_data = reader.get_data();
        PDBViewer viewer(param, pdb_data);
        viewer.show_viewer();
    }

    return 0;    
}