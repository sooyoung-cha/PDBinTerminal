#pragma once
#include <iostream>

struct alignas(16) Atom {
    mutable float mX;
    mutable float mY;
    mutable float mZ;

    Atom(float x, float y, float z) : mX(x), mY(y), mZ(z) {}
    Atom() : mX(0), mY(0), mZ(0) {}

    float* get_position() const {
        static float coords[3]; 
        coords[0] = mX;
        coords[1] = mY;
        coords[2] = mZ;
        return coords;
    }

    void set_position(float x, float y, float z) {
        mX = x;
        mY = y;
        mZ = z;
    }

    void print_position() {
        std::cout << "x: " << mX << ", y: " << mY << ", z: " << mZ << std::endl;
    }
};