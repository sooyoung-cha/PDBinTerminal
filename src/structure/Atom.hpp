#pragma once
#include <iostream>

struct Atom {
    float x;
    float y;
    float z;
    char structure='x';        // 'x' : default, 'h' : helix, 's' : sheet

    Atom(float x_, float y_, float z_) : x(x_), y(y_), z(z_), structure{'*'} {}
    Atom(float x_, float y_, float z_, char c) : x(x_), y(y_), z(z_), structure{c} {}
    Atom() : x(0), y(0), z(0), structure{'*'} {}

    void set_position(float x_, float y_, float z_) {
        x = x_;
        y = y_;
        z = z_;
        return;
    }

    float* get_position() const {
        static float coords[3]; 
        coords[0] = x;
        coords[1] = y;
        coords[2] = z;
        return coords;
    }

    void set_structure(char c){
        structure = c;
        return;
    }

    char get_structure() const {
        return structure;
    }

    void print_position() {
        std::cout << "x: " << x << ", y: " << y << ", z: " << z << std::endl;
        return;
    }
};