#pragma once

#include "../libs/glm/glm.hpp"
#include "../libs/glm/ext/matrix_transform.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"

#include <memory>
#include "mesh.hpp"

namespace lumina
{
    class Object
    {
        public:
        std::shared_ptr<Mesh> mesh;

        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;

        Object(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::shared_ptr<Mesh> mesh)
        : position(position), rotation(rotation), scale(scale), mesh(mesh)
        {

        }

        glm::mat4 getModelMatrix()
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, scale);
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, position);
            return model;
        }
    };
}