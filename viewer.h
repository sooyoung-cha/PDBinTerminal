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

        Chain read_chain(vector<string> struc, char chain_name){
            Chain chain;
            chain.name = chain_name;

            int cnt_atoms = 0;
            for(const string s : struc) {
                Atom new_atom;
                vector<string> s_split = string_split(s);
                if (s_split[4][0] == chain_name){
                    new_atom.element = s_split[2];
                    new_atom.aa = s_split[3];
                    new_atom.coords[x] = stof(s_split[6]);
                    new_atom.coords[y] = stof(s_split[7]);
                    new_atom.coords[z] = stof(s_split[8]);

                    chain.center.coords[x] += new_atom.coords[x];
                    chain.center.coords[y] += new_atom.coords[y];
                    chain.center.coords[z] += new_atom.coords[z];

                    chain.num_atoms++;

                    chain.atoms.push_back(new_atom);
                }

            }
            chain.center.coords[x] /= chain.num_atoms;
            chain.center.coords[y] /= chain.num_atoms;
            chain.center.coords[z] /= chain.num_atoms;
            return chain;
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
        int x_limit;
        int y_limit;
        int x_move = 0;
        int y_move = 0;
        int rot = 0;

        PDBData data;

    public:
        PDBViewer(Parameters& param, PDBData set_data){
            x_limit = param.get_boxsize();
            y_limit = param.get_boxsize();
            data = set_data;

            return;
        }
        
        bool move_atom(Atom& a){
            bool can_move = true;
            if(a.coords[x] + x_move < 0) can_move = false;
            else if(a.coords[x] + x_move > x_limit) can_move = false;
            else if(a.coords[y] + y_move < 0) can_move = false;
            else if(a.coords[y] + y_move > y_limit) can_move = false;

            a.coords[x] += x_move;
            a.coords[y] += y_move;

            return can_move;
        }

        double dist(Atom& a, Atom& b){
            return sqrt(pow((a.coords[x] - b.coords[x]), 2) + pow((a.coords[y] - b.coords[y]), 2));
        }

        void rotate_atom(Atom& a, Atom& center){
            double distance = dist(a, center);
            Atom new_a;
            
            new_a.coords[x] = center.coords[x] + (distance * ((a.coords[x]-center.coords[x]) * cos(rot * 45 * PI / 180.0) - (a.coords[y]-center.coords[y]) * sin(rot * 45 * PI / 180.0)));
            new_a.coords[y] = center.coords[y] + (distance * ((a.coords[y]-center.coords[y]) * cos(rot * 45 * PI / 180.0) + (a.coords[x]-center.coords[x]) * sin(rot * 45 * PI / 180.0)));

            a.coords[x] = new_a.coords[x];
            a.coords[y] = new_a.coords[y];

            return;
        }

        bool apply_user_input(){
            bool keep_show = true;
            switch(getch()){
                // A, a
                case 97:
                case 65:
                    y_move -= 1;
                    break;
                // W, w
                case 119:
                case 87:
                    x_move -= 1;
                    break;
                // D, d
                case 100:
                case 68:
                    y_move += 1;
                    break;
                // S, s
                case 115:
                case 83:
                    x_move += 1;
                    break;      
                // Q, q
                case 113:
                case 81:
                    rot = (rot + 1) % 8;
                    break;  
                // E, e
                case 101:
                case 69:
                    rot = (rot - 1) % 8;
                    break;   
                default:
                    keep_show = false;
                    break;       
            }

            return keep_show;      
        }

        void print_pdb(PDBData& pdb){
            Atom printed_atom;

            for(unsigned int j = 0; j < pdb.get_num_chains(); j++){
                Chain chain = pdb.get_chain(j);
                for (unsigned int i = 0; i < chain.num_atoms; i++){                 
                    printed_atom.coords[x] = chain.atoms[i].coords[x];
                    printed_atom.coords[y] = chain.atoms[i].coords[y];

                    if (rot != 0) { rotate_atom(printed_atom, pdb.get_center()); }
                    if (move_atom(printed_atom)){
                        move(floor(printed_atom.coords[x]), floor(printed_atom.coords[y]));
                        printw("*"); 
                    }
                }
            }
        }

        void show_viewer(){
            char user_input;
            bool keep_show = true;

            initscr(); 
            noecho(); 

            do{
                clear();

                move(0, 0);
                printw("O"); 
                move(x_limit, 0);
                printw("O");
                move(0, y_limit);
                printw("O");
                move(x_limit, y_limit);
                printw("O");

                print_pdb(data);
                refresh();
                keep_show = apply_user_input();                            
            }while(keep_show);

            endwin();
        }
};