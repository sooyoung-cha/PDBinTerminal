#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <vector>

#include <typeinfo>

using namespace std;

class Parameters{
    private:
        string in_file = "./AF-A0A2U9ICS3-F1-model_v4.pdb";
        string format = "pdb";
        string chains = "A";
        int model = 1;
        float boxsize = 100.0;
        bool arg_okay = true;

    public:
        Parameters(int argc, char* argv[]){
            // get in_file
            if (argc > 1){
                in_file = argv[1];
            }
            else {
                cout << "Need input file dir !!!" << endl;
                arg_okay = false;
            }

            // get format, chains, model, boxsize
            for (int i = 2; i < argc; i++){
                try{
                    if (!strcmp(argv[i],"-f") || !strcmp(argv[i],"--format")){
                        if (!strcmp(argv[i+1], "pdb") || !strcmp(argv[i+1], "mmcif") || !strcmp(argv[i+1], "mmtf") || !strcmp(argv[i+1], "mae") || !strcmp(argv[i+1], "maegz")){
                            format = argv[i+1];
                            i++;
                        }
                        else {throw;}
                    }
                    else if (!strcmp(argv[i],"-m") || !strcmp(argv[i],"--model")){
                        model = atoi(argv[i+1]);
                        i++;
                    }
                    else if (!strcmp(argv[i],"-c") || !strcmp(argv[i],"--chains")){
                        chains = argv[i+1];
                        i++;
                    }
                    else if (!strcmp(argv[i],"-s") || !strcmp(argv[i],"--boxsize")){
                        boxsize = atof(argv[i+1]);
                        i++;
                    }
                    else {throw;}
                }
                catch(...){
                    cout << "Wrong input parameters !!!" << endl;
                    cout << argv[i] << endl;
                    arg_okay = false;
                }
            }
        }

        bool check_arg_okay(){
            return arg_okay;
        }

        void print_args(){
            cout << "Input parameters >> " << endl;
            cout << "  in_file: " << in_file << endl;
            cout << "  format: " << format << endl;
            cout << "  model: " << model << endl;
            cout << "  chains: " << chains << endl;
            cout << "  boxsize: " << boxsize << endl;
            return;
        }

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
        float get_boxsize(){
            return boxsize;
        }
};

enum xyz {x, y, z};
struct Atom{
        string element;
        string aa;
        float coords[3];
};

struct Chain{
    char name;
    vector<Atom> atoms;
};

class PDBData{
    private:
        vector<Chain> chains;
    public:
        PDBData(){
            return;
        }

        //get, set
        void set_chain(Chain chain){
            chains.push_back(chain);
            return;
        }
};

class PDBReader{
    private:
        PDBData data;

        vector<string> read_structure(string in_file){
            ifstream openFile(in_file.data()); 
            vector<string> struc;

            if(openFile.is_open()){
                string line;
                while(getline(openFile, line)){
                    if(line.find("ATOM") == 0){
                        struc.push_back(line);
                    }
                }
            }               
            return struc;
        }

        Chain read_chain(vector<string> struc, char chain_name){
            Chain chain;
            chain.name = chain_name;
            for(const string s : struc) {
                Atom new_atom;
                vector<string> s_split = string_split(s);
                if (s_split[4][0] == chain_name){
                    new_atom.element = s_split[2];
                    new_atom.aa = s_split[3];
                    new_atom.coords[0] = stof(s_split[6]);
                    new_atom.coords[1] = stof(s_split[7]);
                    new_atom.coords[2] = stof(s_split[8]);

                    chain.atoms.push_back(new_atom);
                }
                //cout << new_atom.element << ", " << new_atom.aa << ", " << new_atom.coords[0] << ", " << new_atom.coords[1] << ", " << new_atom.coords[2] << endl; 
            }
            return chain;
        }

        vector<string> string_split(string s){
            string temp = "";
            vector<string> result;
            
            for(int i = 0; i < s.length(); i++){
                if (s[i] == ' ' && temp != ""){
                    result.push_back(temp);
                    temp = "";
                }
                else if(s[i] != ' '){
                    temp = temp + s[i];
                }
            }
            return result;
        }

    public:
        PDBReader(){
            return;
        }

        void read_file(Parameters& param){
            string in_file = param.get_in_file();
            string chains = param.get_chains();

            vector<string> struc = read_structure(in_file);
            for(int i = 0; i < chains.length(); i++){
                data.set_chain(read_chain(struc, chains[i]));
            }
        }

        // get, set
        PDBData get_data(){
            return data;
        }
};

class PDBViewer{
    private:

    public:
        PDBViewer(){

        }

        void print_data(Parameters param, PDBData data){
            
        }
};