#pragma once

#include <cstdlib>
#include <random>

namespace lumina
{
    double getRandomFloat(float lower, float upper)
    {
        return lower + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(upper-lower)));
    }

    void relative2Pixel(float relative_x, float relative_y, int w, int h, int& x, int& y)
    {
        x = relative_x * w;
        y = relative_y * h;
    }
}