#pragma once

#include "utils.hpp"

namespace lumina {
namespace ui
{

class Button : public Element
{
    std::string label;
    std::function<void()> on_click;
    std::shared_ptr<lumina::Shader>& shader_ptr;
    std::shared_ptr<lumina::Font>& font_ptr;
    std::shared_ptr<lumina::Mesh>& mesh_ptr;
    glm::vec4 color;
    glm::vec4 text_color;
    glm::vec4 hover_color;
    glm::vec4 highlight_color;
    bool is_hovered = false;

    public:
    Button(std::string name, float min_x, float min_y, float max_x, float max_y, const std::string& label, std::shared_ptr<lumina::Shader>& shader_ptr, std::shared_ptr<lumina::Font>& font_ptr, std::shared_ptr<lumina::Mesh>& mesh_ptr, glm::vec4& color, glm::vec4& text_color, glm::vec4& hover_color, glm::vec4& highlight_color)
    : Element(name, min_x, min_y, max_x, max_y), label(label), shader_ptr(shader_ptr), font_ptr(font_ptr), mesh_ptr(mesh_ptr), color(color), text_color(text_color), hover_color(hover_color), highlight_color(highlight_color)
    {
        
    }

    void render(int window_width, int window_height) override
    {
        if (!active) return;
        glm::vec4 bounds = getAbsoluteBounds();
        glm::vec4 current_color = is_hovered ? hover_color : color;
        float delta_x = bounds.z - bounds.x;
        float delta_y = bounds.w - bounds.y;
        float ratio = float(window_height) / float(window_width);
        if (is_hovered) drawUIRectangle(bounds.x - (0.01f * ratio), bounds.y - 0.01f, bounds.z + (0.01f * ratio), bounds.w + 0.01f, shader_ptr, mesh_ptr, highlight_color, window_width, window_height);
        drawUIRectangle(bounds.x, bounds.y, bounds.z, bounds.w, shader_ptr, mesh_ptr, current_color, window_width, window_height);
        //drawText(label, bounds.x + 0.05f, bounds.y + 0.05f);
        
        glm::vec2 text_size = font_ptr->measureString(label.c_str());

        float text_x = (bounds.x + bounds.z) / 2.0f * window_width;
        float text_y = (bounds.y + bounds.w) / 2.0f * window_height;

        text_x -= text_size.x / 2.0f;
        text_y += text_size.y / 2.0f;

        font_ptr->shader_ptr->bind();

        glUniform4f(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_color"), 1.0f, 1.0f, 1.0f, 1.0f);
        glm::mat4 ortho = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_modelViewProj"), 1, GL_FALSE, glm::value_ptr(ortho));
        font_ptr->drawString(text_x, text_y, label.c_str(), text_color);
        font_ptr->shader_ptr->unbind();

        Element::render(window_width, window_height);
    }

    void handleEvent(SDL_Event& event, int window_width, int window_height) override
    {
        if (!active) return;
        glm::vec4 bounds = getAbsoluteBounds();
        if (event.type == SDL_MOUSEMOTION)
        {
            int mouse_x = event.motion.x;
            int mouse_y = event.motion.y;
            is_hovered = isPointInside(mouse_x, mouse_y, bounds, window_width, window_height);
        }
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (is_hovered && on_click) on_click();
        }
        Element::handleEvent(event, window_width, window_height);
    }

    void setOnClick(const std::function<void()>& callback)
    {
        on_click = callback;
    }

    void setLabel(std::string _label)
    {
        label = _label;
    }
};

}
}