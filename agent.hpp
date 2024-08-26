#include "lumina.hpp"

#include "energy.hpp"

class Agent
{
    public:

    glm::vec3 position;
    float rotation;

    float weight;

    Agent()
    {
        position = glm::vec3(getRandomFloat(-100.0, 100.0), getRandomFloat(-100.0, 100.0), 0.0f);
        rotation = getRandomFloat(0.0f, 360.0f);
    }

    void tick()
    {

    }

    void update(float delta_time, std::vector<Energy> &energy_list)
    {
        float factor = delta_time / 10.0f;
        float forward_speed = 100;
        float min = -0.1f;
        float max = 0.1f;

        float radians = rotation * M_PI / 180;
        glm::vec3 forward_vector = glm::vec3(cos(radians), sin(radians), 0.0f);

        bool detect = false;

        auto it = energy_list.begin();
        while (it != energy_list.end())
        {
            //std::cout << "Hello" << std::endl;
            float distance = glm::distance(it->position, position);
            //std::cout << "Distance: " << distance << std::endl;
            if (distance < 10.0f && glm::dot(it->position, position) > 0)
            {
                detect = true;
                it++;
            }
            else if (distance < 1.0f)
            {
                it = energy_list.erase(it);
            }
            else
            {
                it++;
            }
        }

        /*for (auto &energy : energy_list)
        {
            if (glm::distance(energy.position, position) < 0.05)
            {
                delete(&energy);
            }
        }*/

        position += forward_vector * forward_speed * factor * 0.001f;

        if (!detect)
        {
            rotation -= 1.0f * factor;   
        }
        rotation = std::fmod(rotation, 360.0f);
    }
};