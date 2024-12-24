#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib> // For malloc and free
#include <ncurses.h>


struct Atom {
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
        char* result;

        // Calculate the required size for the string
        int size = snprintf(nullptr, 0, "x: %.2f, y: %.2f, z: %.2f", mX, mY, mZ) + 1;

        // Allocate memory dynamically
        result = (char*)malloc(size);

        // Generate the string
        snprintf(result, size, "x: %.2f, y: %.2f, z: %.2f", mX, mY, mZ);
        mvprintw(LINES - 1, 0, result); 
        // std::cout << "x: " << mX << ", y: " << mY << ", z: " << mZ << std::endl;
    }
};