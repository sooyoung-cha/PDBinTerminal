#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

int main()
{
  int chain_length = 0;
  char ifbuffer[500];
  float ca_coords[2000][3];
  string str;
  ifstream pdb_reader_stream;

  pdb_reader_stream.open("./data/AF-A0A2U9ICS3-F1-model_v4.pdb");

  while (!pdb_reader_stream.eof()) {
    getline(pdb_reader_stream, str);

    if (str.substr(13, 3) == "CA ") {

      float x = stof(str.substr(32, 6));
      float y = stof(str.substr(40, 6));
      float z = stof(str.substr(48, 6));

      ca_coords[chain_length][0] = x;
      ca_coords[chain_length][1] = y;
      ca_coords[chain_length][2] = z;
      chain_length++;
    }
  }

  for (int i = 0; i < chain_length; i++) {
    cout << i << ": " << ca_coords[i][0] << " " << ca_coords[i][1] << " " << ca_coords[i][2] << endl;
  }

  pdb_reader_stream.close();
}
