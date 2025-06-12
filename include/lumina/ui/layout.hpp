#pragma once

#include "ui.hpp"

namespace lumina {    
namespace ui
{
class HorizontalLayout : public Element
{
public:
    float gap = 0.0f;
    HorizontalLayout(std::string name, float min_x, float min_y, float max_x, float max_y, float gap)
    : Element(name, min_x, min_y, max_x, max_y), gap(gap)
    {
        
    }

    void updateChildDimensions()
    {
        std::size_t count = children.size();
        float total_gap = (count > 1) ? gap * (count - 1) : 0.0f;
        float available_space = 1.0f - total_gap;
        float item_size = available_space / static_cast<float>(count);
        int i = 0;
        for (const auto& [name, element] : children)
        {
            float start = i * (item_size + gap);
            float end = start + item_size;
            element->min_x = start;
            element->max_x = end;
            element->min_y = 0.0f;
            element->max_y = 1.0f;
            i++;
        }
    }
};

class VerticalLayout : public Element
{
public:
    float gap = 0.0f;
    VerticalLayout(std::string name, float min_x, float min_y, float max_x, float max_y, float gap)
    : Element(name, min_x, min_y, max_x, max_y), gap(gap)
    {
        
    }

    void updateChildDimensions()
    {

        std::size_t count = children.size();
        float total_gap = (count > 1) ? gap * (count - 1) : 0.0f;
        float available_space = 1.0f - total_gap;
        float item_size = available_space / static_cast<float>(count);
        int i = 0;
        for (const auto& [name, element] : children)
        {
            float start = i * (item_size + gap);
            float end = start + item_size;
            element->min_x = 0.0f;
            element->max_x = 1.0f;
            element->min_y = start;
            element->max_y = end;
            i++;
        }
    }
};
}
}