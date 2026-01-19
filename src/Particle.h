#pragma once
#include <iostream>
#include <vector>
#include "cxy.h"


struct Particle
{
    cxy pos;
    cxy vel;
    float density;

    Particle();

    Particle(int w, int h);

    static
    std::vector<Particle>
    generate(
        int count,
        int w, int h,
    const cxy& offset);

    std::string text() const;

    void test();
};


