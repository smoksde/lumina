#pragma once

#include <SDL2/SDL.h>


#include <memory>
#include <vector>
#include <algorithm>

#include "lumina/ui/element.hpp"

namespace lumina::ui
{

class System
{
public:
    System() = default;

    void add(const std::shared_ptr<Element>& element)
    {
        elements.push_back(element);
    }

    void remove(const std::shared_ptr<Element>& element)
    {
        elements.erase(
            std::remove(elements.begin(), elements.end(), element),
            elements.end()
        );
    }

    void clear()
    {
        elements.clear();
    }

    void render(int width, int height)
    {
        for (auto& element : elements)
        {
            element->render(width, height);
        }
    }

    void handleEvent(SDL_Event& event, int w, int h)
    {
        for (auto& element : elements)
        {
            element->handleEvent(event, w, h);
            // stop looping if the event was consumed
            // (not implemented yet)
        }
    }

    bool isMouseOverUI(int mx, int my, int w, int h) const
    {
        for (const auto& element : elements)
        {
            if (element->isMouseOver(mx, my, w, h))
            {
                return true;
            }
        }
        return false;
    }

private:
    std::vector<std::shared_ptr<Element>> elements;
};
}