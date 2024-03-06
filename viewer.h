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
            return;
        }

        void print_data(Parameters param, PDBData data){
            
        }

        int* get_projection(Atom a, float* center_coor){
            float proj_coor[3];
            float proj_x = a.coords[x];
            float prot_y = a.coords[y];
            float prot_y = a.coords[y];

            return ;
        }

        float* calculate_center(vector<Atom>& atoms){
            float center_coor[3] = {0.0, 0.0, 0.0};
            for(const Atom a : atoms) {
                center_coor[x] += a.coords[x];
                center_coor[y] += a.coords[y];
                center_coor[z] += a.coords[z];
            }
            center_coor[x]/=atoms.size();
            center_coor[y]/=atoms.size();
            center_coor[z]/=atoms.size();

            return center_coor;
        }

        void print_atoms(Chain c){
            int* proj_coor;
            float* center_coor = calculate_center(c.atoms);

            initscr(); // ncurses 세션을 시작합니다.
            for(const Atom a : c.atoms) {
                proj_coor = get_projection(a, center_coor);
                move(proj_coor[x], proj_coor[y]); // 커서를 새로운 위치로 이동합니다.
                printw("*"); // 새 위치에서 문자열을 출력합니다.
            }
            refresh(); // 화면에 변경 사항을 적용합니다.
            getch(); // 사용자 입력을 대기합니다.

            endwin(); // ncurses 세션을 종료합니다.
        }
};