#pragma once

#include "utils.hpp"

namespace lumina {

class UIElement : public std::enable_shared_from_this<UIElement>
{
    protected:
    bool active = true;
    std::string name = "";
    std::shared_ptr<UIElement> parent;
    std::unordered_map<std::string, std::shared_ptr<UIElement>> children;

    public:

    float min_x, min_y, max_x, max_y;
    UIElement(std::string name, float min_x, float min_y, float max_x, float max_y)
    : name(name), min_x(min_x), min_y(min_y), max_x(max_x), max_y(max_y)
    {
    }

    template <typename T>
    void addChild(std::shared_ptr<T> child)
    {
        if (!child) return;
        if (children.find(child->name) != children.end())
        {
            throw std::runtime_error("Child with ID/Name '" + child->name + "' already exists!");
        }

        children[child->name] = std::static_pointer_cast<UIElement>(child);
        child->parent = shared_from_this();
    }

    template <typename T>
    std::shared_ptr<T> getChild(std::string name)
    {
        auto it = children.find(name);
        if (it != children.end())
        {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        throw std::runtime_error("Child with name '" + name + "' does not exist!");
    }

    void setActive(bool new_active)
    {
        active = new_active;
    }

    bool isActive()
    {
        return active;
    }

    virtual void render(int window_width, int window_height)
    {
        if (!active) return;
        for (const auto& [name, child] : children) child->render(window_width, window_height);
    }

    virtual void handleEvent(SDL_Event& event, int window_width, int window_height)
    {
        if (!active) return;
        for (const auto& [name, child] : children) child->handleEvent(event, window_width, window_height);
    }

    glm::vec4 getAbsoluteBounds()
    {
        float abs_min_x = min_x, abs_min_y = min_y, abs_max_x = max_x, abs_max_y = max_y;
        if (parent)
        {
            auto parent_bounds = parent->getAbsoluteBounds();
            abs_min_x = parent_bounds.x + (parent_bounds.z - parent_bounds.x) * min_x;
            abs_min_y = parent_bounds.y + (parent_bounds.w - parent_bounds.y) * min_y;
            abs_max_x = parent_bounds.z - (parent_bounds.z - parent_bounds.x) * (1.0f - max_x);
            abs_max_y = parent_bounds.w - (parent_bounds.w - parent_bounds.y) * (1.0f - max_y);
        }
        return {abs_min_x, abs_min_y, abs_max_x, abs_max_y};
    }
};

}