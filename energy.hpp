#pragma once

#include "lumina.hpp"

class Energy
{
    public:

    glm::vec3 position;

    Energy()
    {
        position = glm::vec3(getRandomFloat(-100.0, 100.0), getRandomFloat(-100.0, 100.0), 0.0f);
    }

    void tick()
    {

    }

    void update(float delta_time)
    {

    }
};