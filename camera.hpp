#pragma once

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include "libs/glm/gtc/type_ptr.hpp"

class Camera
{
    public:

    float width;
    float height;
    float zoom;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;

    Camera(float width, float height)
    : width(width), height(height)
    {
        zoom = 1.0f;
        projection = glm::ortho(-zoom * width / 2.0f, 
                                zoom * width / 2.0f,
                                -zoom * height / 2.0f,
                                zoom * height / 2.0f,
                                -1.0f,
                                1000.0f
                                );
    }

    void translate(glm::vec3 translation_vector)
    {
        position += translation_vector * -1.0f;
    }

    float getX(){return position[0];}
    
    float getY(){return position[1];}
    
    float getZ(){return position[2];}
};