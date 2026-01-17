#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace lumina
{
    class Mesh;
    class Material;

    struct Renderable
    {
        std::shared_ptr<Mesh> mesh = nullptr;
        std::shared_ptr<Material> material = nullptr;
        glm::mat4 model { 1.0f };

        Renderable() = default;

        Renderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4& model)
        : mesh(mesh), material(material), model(model)
        {

        }
    };
}