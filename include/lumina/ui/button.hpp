#pragma once

#include "utils.hpp"
#include "context.hpp"
#include "action.hpp"

namespace lumina {
namespace ui
{

class Button : public Element
{
    std::string label;
    std::shared_ptr<lumina::Texture> icon;

    std::function<void()> on_click;
    std::shared_ptr<lumina::Shader>& shader_ptr;
    std::shared_ptr<lumina::Shader>& texture_shader_ptr;
    std::shared_ptr<lumina::Font>& font_ptr;
    std::shared_ptr<lumina::Mesh>& mesh_ptr;
    glm::vec4 color;
    glm::vec4 text_color;
    glm::vec4 hover_color;
    glm::vec4 highlight_color;

    public:
    Button(std::string name, float min_x, float min_y, float max_x, float max_y, const std::string& label, lumina::ui::Context& style, glm::vec4& color, glm::vec4& text_color, glm::vec4& hover_color, glm::vec4& highlight_color)
    : Element(name, min_x, min_y, max_x, max_y), 
    label(label),
    shader_ptr(style.shader_ptr),
    texture_shader_ptr(style.texture_shader_ptr),
    font_ptr(style.font_ptr),
    mesh_ptr(style.mesh_ptr),
    color(color),
    text_color(text_color),
    hover_color(hover_color),
    highlight_color(highlight_color)
    {
        
    }

    void setIcon(std::shared_ptr<lumina::Texture> tex)
    {
        icon = tex;
    }

    void render(int window_width, int window_height) override
    {
        if (!active) return;
        glm::vec4 bounds = getAbsoluteBounds();
        glm::vec4 current_color = is_hovered ? hover_color : color;
        float ratio = float(window_height) / float(window_width);
        if (is_hovered) drawUIRectangle(bounds.x - (0.01f * ratio), bounds.y - 0.01f, bounds.z + (0.01f * ratio), bounds.w + 0.01f, shader_ptr, mesh_ptr, highlight_color, window_width, window_height);
        drawUIRectangle(bounds.x, bounds.y, bounds.z, bounds.w, shader_ptr, mesh_ptr, current_color, window_width, window_height);
        
        
        // Draw icon with correct aspect ratio and pixel-perfect scaling
        if (icon)
        {
            int tex_w = icon->getWidth();
            int tex_h = icon->getHeight();

            // Button size in pixels
            float button_w_px = (bounds.z - bounds.x) * window_width;
            float button_h_px = (bounds.w - bounds.y) * window_height;

            // Scale factor to fit texture inside button
            float scale = std::min(button_w_px / tex_w, button_h_px / tex_h);

            float draw_w_px = tex_w * scale;
            float draw_h_px = tex_h * scale;

            // Center in button
            float center_x_px = (bounds.x + bounds.z) * 0.5f * window_width;
            float center_y_px = (bounds.y + bounds.w) * 0.5f * window_height;

            // Compute normalized coordinates for drawUITexture
            float draw_min_x = (center_x_px - draw_w_px / 2.0f) / window_width;
            float draw_max_x = (center_x_px + draw_w_px / 2.0f) / window_width;
            float draw_min_y = (center_y_px - draw_h_px / 2.0f) / window_height;
            float draw_max_y = (center_y_px + draw_h_px / 2.0f) / window_height;

            drawUITexture(draw_min_x, draw_min_y, draw_max_x, draw_max_y,
                        texture_shader_ptr, mesh_ptr, icon, window_width, window_height);
        }
        else
        {
            drawText(label, bounds, *font_ptr, text_color, window_width, window_height, TextAlign::Center);
        }

        Element::render(window_width, window_height);
    }

    void handleEvent(SDL_Event& event, int window_width, int window_height) override
    {
        if (!active) return;
        
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

    void setOnClick(const std::string& action_name)
    {
        on_click = lumina::ui::ActionRegistry::instance().get(action_name);
    }

    void setLabel(std::string _label)
    {
        label = _label;
    }
};

}
}