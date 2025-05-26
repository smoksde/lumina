#pragma once

#include <vector>
#include <string>
#include <functional>
#include "element.hpp"
#include "button.hpp"
#include "utils.hpp"

namespace lumina
{
enum TextAlignment
{
    CENTERED,
    LEFT
};

class UICanvas : public UIElement
{
    public:
    std::shared_ptr<lumina::Shader>& shader_ptr;
    std::shared_ptr<lumina::Mesh>& mesh_ptr;
    glm::vec4 color;
    UICanvas(std::string name, float min_x, float min_y, float max_x, float max_y, std::shared_ptr<lumina::Shader>& shader_ptr, std::shared_ptr<lumina::Mesh>& mesh_ptr, glm::vec4& color)
    : UIElement(name, min_x, min_y, max_x, max_y), shader_ptr(shader_ptr), mesh_ptr(mesh_ptr), color(color)
    {

    }

    void render(int window_width, int window_height) override
    {
        if (!active) return;
        glm::vec4 bounds = getAbsoluteBounds();
        drawUIRectangle(bounds.x, bounds.y, bounds.z, bounds.w, shader_ptr, mesh_ptr, color, window_width, window_height);

        UIElement::render(window_width, window_height);
    }

    void handleEvent(SDL_Event& event, int window_width, int window_height) override
    {
        if (!active) return;
        UIElement::handleEvent(event, window_width, window_height);
    }
};


class UIText : public UIElement
{
    std::string text = "";
    TextAlignment text_alignment = TextAlignment::CENTERED;
    glm::vec2 text_padding;
    std::shared_ptr<lumina::Shader>& shader_ptr;
    std::shared_ptr<lumina::Font>& font_ptr;
    glm::vec4 text_color;

    public:
    
    UIText(std::string name, float min_x, float min_y, float max_x, float max_y,
    std::shared_ptr<lumina::Shader>& shader_ptr,
    std::shared_ptr<lumina::Font>& font_ptr,
    glm::vec4 text_color
    )
    : UIElement(name, min_x, min_y, max_x, max_y),
    shader_ptr(shader_ptr),
    font_ptr(font_ptr),
    text_color(text_color)
    {

    }

    void render(int window_width, int window_height) override
    {
        if (!active) return;

        glm::vec4 bounds = getAbsoluteBounds();
        glm::vec2 text_size = font_ptr->measureString(text.c_str());
        float text_x;
        float text_y;
        if (text_alignment == TextAlignment::CENTERED)
        {
            text_x = (bounds.x + bounds.z) / 2.0f * window_width;
            text_y = (bounds.y + bounds.w) / 2.0f * window_height;
            text_x -= text_size.x / 2.0f;
            text_y += text_size.y / 2.0f;
        }
        else if (text_alignment == TextAlignment::LEFT)
        {
            text_x = bounds.x * window_width;
            text_y = (bounds.y + bounds.w) / 2.0f * window_height;
            text_y += text_size.y / 2.0f;
        }
        
        text_x += text_padding.x;
        text_y += text_padding.y;

        font_ptr->shader_ptr->bind();
        glUniform4f(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_color"), 1.0f, 1.0f, 1.0f, 1.0f);
        glm::mat4 ortho = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_modelViewProj"), 1, GL_FALSE, glm::value_ptr(ortho));
        font_ptr->drawString(text_x, text_y, text.c_str(), text_color);
        font_ptr->shader_ptr->unbind();

        UIElement::render(window_width, window_height);
    }

    void setText(std::string new_text)
    {
        text = new_text;
    }

    void setTextAlignment(TextAlignment new_alignment)
    {
        text_alignment = new_alignment;
    }

    void setTextPadding(glm::vec2 new_padding)
    {
        text_padding = new_padding;
    }
};

class UITextLines : public UIElement
{
    public:
    std::vector<std::string> lines;
    std::shared_ptr<lumina::Shader>& shader_ptr;
    std::shared_ptr<lumina::Font>& font_ptr;
    glm::vec4 text_color;
    float line_spacing = 1.2f;
    bool focused = false;

    UITextLines(std::string name, float min_x, float min_y, float max_x, float max_y,
    std::shared_ptr<lumina::Shader>& shader_ptr, std::shared_ptr<lumina::Font>& font_ptr,
    glm::vec4 text_color)
    : UIElement(name, min_x, min_y, max_x, max_y),
    shader_ptr(shader_ptr),
    font_ptr(font_ptr),
    text_color(text_color)
    {

    }

    void render(int window_width, int window_height)
    {
        if (!active) return;
        glm::vec4 bounds = getAbsoluteBounds();
        float start_x = bounds.x * window_width;
        float start_y = bounds.y * window_height + 100.0f;
        float current_y = start_y;

        font_ptr->shader_ptr->bind();
        glm::mat4 ortho = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_modelViewProj"), 1, GL_FALSE, glm::value_ptr(ortho));
        glUniform4f(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_color"), text_color.r, text_color.g, text_color.b, text_color.a);

        for (const std::string& line : lines)
        {
            glm::vec2 text_size = font_ptr->measureString(line.c_str());
            float text_x = start_x;
            font_ptr->drawString(text_x, current_y, line.c_str(), text_color);
            current_y += text_size.y * line_spacing;
        }

        font_ptr->shader_ptr->unbind();
        UIElement::render(window_width, window_height);
    }

    void handleEvent(SDL_Event& event, int window_width, int window_height)
    {
        if (!active) return;

        glm::vec4 bounds = getAbsoluteBounds();
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouse_x = event.button.x;
            int mouse_y = event.button.y;
            focused = isPointInside(mouse_x, mouse_y, bounds, window_width, window_height);
        }
        if (focused && event.type == SDL_TEXTINPUT)
        {
            if (lines.empty())
            {
                lines.push_back(event.text.text);
                
            }
            else
            {
                lines.back() += event.text.text;
            }
        }
        if (focused && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && !lines.back().empty())
        {
            lines.back().pop_back();
        }
        UIElement::handleEvent(event, window_width, window_height);
    }
};

class UITextField : public UIElement
{
    std::string text;
    glm::vec4 color;
    glm::vec4 text_color;
    glm::vec4 hover_color;
    std::shared_ptr<lumina::Shader>& shader_ptr;
    std::shared_ptr<lumina::Font>& font_ptr;
    std::shared_ptr<lumina::Mesh>& mesh_ptr;
    bool focused = false;
    float caret_timer = 0.0f;
    bool show_caret = true;

    public:
    
    UITextField(std::string name, float min_x, float min_y, float max_x, float max_y,
    std::shared_ptr<lumina::Shader>& shader_ptr,
    std::shared_ptr<lumina::Font>& font_ptr,
    std::shared_ptr<lumina::Mesh>& mesh_ptr,
    glm::vec4& color,
    glm::vec4& text_color,
    glm::vec4& hover_color
    )
    : UIElement(name, min_x, min_y, max_x, max_y),
    shader_ptr(shader_ptr),
    font_ptr(font_ptr),
    mesh_ptr(mesh_ptr),
    color(color),
    text_color(text_color),
    hover_color(hover_color)
    {

    }

    void render(int window_width, int window_height) override
    {
        if (!active) return;

        glm::vec4 bounds = getAbsoluteBounds();
        drawUIRectangle(bounds.x, bounds.y, bounds.z, bounds.w, shader_ptr, mesh_ptr, color, window_width, window_height);

        glm::vec2 text_size = font_ptr->measureString(text.c_str());
        float text_x = (bounds.x + bounds.z) / 2.0f * window_width;
        float text_y = (bounds.y + bounds.w) / 2.0f * window_height;
        text_x -= text_size.x / 2.0f;
        text_y += text_size.y / 2.0f;
        font_ptr->shader_ptr->bind();
        glUniform4f(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_color"), 1.0f, 1.0f, 1.0f, 1.0f);
        glm::mat4 ortho = glm::ortho(0.0f, (float)window_width, (float)window_height, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_modelViewProj"), 1, GL_FALSE, glm::value_ptr(ortho));
        font_ptr->drawString(text_x, text_y, text.c_str(), text_color);
        font_ptr->shader_ptr->unbind();

        UIElement::render(window_width, window_height);
    }

    void handleEvent(SDL_Event& event, int window_width, int window_height) override
    {
        if (!active) return;

        glm::vec4 bounds = getAbsoluteBounds();
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouse_x = event.button.x;
            int mouse_y = event.button.y;
            focused = isPointInside(mouse_x, mouse_y, bounds, window_width, window_height);
            
        }
        if (focused && event.type == SDL_TEXTINPUT)
        {
            text += event.text.text;
        }
        if (focused && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && !text.empty())
        {
            text.pop_back();
        }

        UIElement::handleEvent(event, window_width, window_height);
    }

    std::string getText()
    {
        return text;
    }
};

   
}