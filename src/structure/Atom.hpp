#pragma once

class Atom{
public:
  Atom(float, float, float);
  Atom();
  float* get_position();
  void set_position(float , float , float );
  void print_position();
private:
  float mX;
  float mY;
  float mZ;
};