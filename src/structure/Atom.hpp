#pragma once
#include <iostream>

struct Atom {
    float mX;
    float mY;
    float mZ;
    char mStructure;        // 'x' : default, 'h' : helix, 's' : sheet

    Atom(float x, float y, float z) : mX(x), mY(y), mZ(z), mStructure{'*'} {}
    Atom(float x, float y, float z, char c) : mX(x), mY(y), mZ(z), mStructure{c} {}
    Atom() : mX(0), mY(0), mZ(0), mStructure{'*'} {}

    void set_position(float x, float y, float z) {
        mX = x;
        mY = y;
        mZ = z;
        return;
    }

    float* get_position() const {
        static float coords[3]; 
        coords[0] = mX;
        coords[1] = mY;
        coords[2] = mZ;
        return coords;
    }

    void set_structure(char c){
        mStructure = c;
        return;
    }

    char get_structure() const {
        return mStructure;
    }

    void print_position() {
        std::cout << "x: " << mX << ", y: " << mY << ", z: " << mZ << std::endl;
        return;
    }
};