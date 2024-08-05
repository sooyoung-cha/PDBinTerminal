#pragma once

class Atom{
public:
  Atom(float, float, float);
  float* get_position();
  void set_position(float , float , float );
private:
  float x;
  float y;
  float z;
};