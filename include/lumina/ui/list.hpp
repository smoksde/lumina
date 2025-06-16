#pragma once

#include <sstream>
#include <iomanip> 

namespace lumina
{
namespace ui
{

class List : public Element
{
private:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Font> font;
    std::shared_ptr<Mesh> mesh;

    float gap = 0.0f;
    float min_item_height = 0.1f;
    float scroll_offset = 0.0f;
public:
    List(std::string name, float min_x, float min_y, float max_x, float max_y, std::shared_ptr<Shader> shader, std::shared_ptr<Font> font, std::shared_ptr<Mesh> mesh)
    : Element(name, min_x, min_y, max_x, max_y), shader(shader), font(font), mesh(mesh)
    {

    }

    void render(int w, int h) override
    {
        if (!active) return;
        glm::vec4 bounds = getAbsoluteBounds();
        glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        drawUIRectangle(bounds.x, bounds.y, bounds.z, bounds.w, shader, mesh, color, w, h);
    
        glEnable(GL_SCISSOR_TEST);
        glScissor(bounds.x * w, (1.0f - bounds.w) * h, (bounds.z - bounds.x) * w, (bounds.w - bounds.y) * h);

        for (auto& [name, child] : children)
        {
            child->render(w, h);
        }

        glDisable(GL_SCISSOR_TEST);
    }

    void handleEvent(SDL_Event& event, int w, int h) override
    {
        if (!active) return;

        switch (event.type)
        {
            case SDL_MOUSEWHEEL:
            {
                if (!is_hovered) break;
                float diff = -event.wheel.preciseY;
                scroll_offset += diff * 0.02f;
                scroll_offset = std::clamp(scroll_offset, 0.0f, getTotalListHeight() - getTotalHeight());
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {

                }   
                break;
            } 
            default:
                break;
        }

        updateChildDimensions();
        Element::handleEvent(event, w, h);
    }

    void updateChildDimensions()
    {
        std::size_t count = children.size();
        if (count == 0) return;
        glm::vec4 bounds = getAbsoluteBounds();
        float item_height = getItemHeight();

        float y = bounds.y - scroll_offset;

        for (auto& [name, child] : children)
        {
            float child_min_y = y;
            float child_max_y = y + item_height;

            child->min_x = min_x;
            child->max_x = max_x;
            child->min_y = (child_min_y - bounds.y) / getTotalHeight();
            child->max_y = (child_max_y - bounds.y) / getTotalHeight();

            y += item_height + gap;
        }
    }

    // Height of the visible part
    float getTotalHeight()
    {
        glm::vec4 bounds = getAbsoluteBounds();
        return bounds.w - bounds.y;
    }

    float getAvailableHeight()
    {
        return getTotalHeight() - getTotalGap();
    }

    float getTotalGap()
    {
        std::size_t count = children.size();
        return (count > 1) ? gap * (count - 1) : 0.0f;
    }

    float getItemHeight()
    {
        std::size_t count = children.size();
        return std::max(getAvailableHeight() / static_cast<float>(count), min_item_height);
    }

    // Height of the rolled out list including the invisible part
    float getTotalListHeight()
    {
        std::size_t count = children.size();
        float item_height = getItemHeight();
        return (item_height * count) + getTotalGap();
    }

    std::string vec4ToString(const glm::vec4& v, int precision = 2)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision);
        oss << v.x << ", " << v.y << ", " << v.z << ", " << v.w;
        return oss.str();
    }
};

}
}