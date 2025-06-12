#pragma once

#include "utils.hpp"

namespace lumina {
namespace ui
{
class Canvas : public Element
{
    public:
    std::shared_ptr<lumina::Shader>& shader_ptr;
    std::shared_ptr<lumina::Mesh>& mesh_ptr;
    glm::vec4 color;
    Canvas(std::string name, float min_x, float min_y, float max_x, float max_y, std::shared_ptr<lumina::Shader>& shader_ptr, std::shared_ptr<lumina::Mesh>& mesh_ptr, glm::vec4& color)
    : Element(name, min_x, min_y, max_x, max_y), shader_ptr(shader_ptr), mesh_ptr(mesh_ptr), color(color)
    {

    }

    void render(int window_width, int window_height) override
    {
        if (!active) return;
        glm::vec4 bounds = getAbsoluteBounds();
        drawUIRectangle(bounds.x, bounds.y, bounds.z, bounds.w, shader_ptr, mesh_ptr, color, window_width, window_height);

        Element::render(window_width, window_height);
    }

    void handleEvent(SDL_Event& event, int window_width, int window_height) override
    {
        if (!active) return;
        Element::handleEvent(event, window_width, window_height);
    }
};

}
}