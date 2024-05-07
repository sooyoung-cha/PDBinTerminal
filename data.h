enum xyz {x, y, z};
struct Atom{
        string element = " ";
        string aa = " ";
        double coords[3] = {0.0, 0.0, 0.0};
};

struct Chain{
    char name;
    vector<Atom> atoms;
    //Atom center;
    unsigned int num_atoms = 0;
};

class PDBData{
    private:
        vector<Chain> chains;
        Atom center;
        unsigned int num_chains = 0;
    public:

        PDBData(){
            center.coords[x] = 0;
            center.coords[y] = 0;
            center.coords[z] = 0;

            return;
        }

        /*
        void update_center(){
            center.coords[x] = 0;
            center.coords[y] = 0;
            center.coords[z] = 0;

            for (const Chain& c : chains){
                center.coords[x] += c.center.coords[x];
                center.coords[y] += c.center.coords[y];
                center.coords[z] += c.center.coords[z];
            }

            center.coords[x] /= num_chains;
            center.coords[y] /= num_chains;
            center.coords[z] /= num_chains;
        }
        */

        void normalize_coords(Atom& norm_factor){
            for (Chain& c : chains){
                for (Atom& a : c.atoms){
                    a.coords[x] /= norm_factor.coords[x];
                    a.coords[y] /= norm_factor.coords[y];
                    a.coords[z] /= norm_factor.coords[z];
                }
            }            

            return;
        }
        
        //get, set
        void set_chain(Chain chain){
            chains.push_back(chain);
            num_chains ++;
            //update_center();
            return;
        }        

        Atom& get_center(){
            return center;
        }

        Chain& get_chain(int idx){
            if (idx < num_chains){
                return chains[idx];
            }
            //else {return;}
        }

        int get_num_chains(){
            return num_chains;
        }
};