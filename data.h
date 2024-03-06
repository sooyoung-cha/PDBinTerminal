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