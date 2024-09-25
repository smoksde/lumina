#pragma once

#include <cstdlib>
#include <random>

namespace lumina
{
    double getRandomFloat(float lower, float upper)
    {
        return lower + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(upper-lower)));
    }
}