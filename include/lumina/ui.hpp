#pragma once

#include <vector>
#include <string>
#include <functional>

enum TextAlignment
{
    CENTERED,
    LEFT
};

inline bool isPointInside(int mouse_x, int mouse_y, glm::vec4& bounds, int window_width, int window_height)
{
    float gl_mouse_x = float(mouse_x) / float(window_width);
    float gl_mouse_y = float(mouse_y) / float(window_height);
    float min_x = bounds.x;
    float min_y = bounds.y;
    float max_x = bounds.z;
    float max_y = bounds.w;
    return gl_mouse_x >= min_x && gl_mouse_x <= max_x && gl_mouse_y >= min_y && gl_mouse_y <= max_y;
}

inline void drawUIRectangle(float min_x, float min_y, float max_x, float max_y, std::shared_ptr<lumina::Shader>& shader_ptr, std::shared_ptr<lumina::Mesh>& mesh_ptr, glm::vec4& color, int window_width, int window_height)
{
    shader_ptr->bind();
    mesh_ptr->bind();

    int color_location = glGetUniformLocation(shader_ptr->shader_id, "color");
    glUniform4fv(color_location, 1, glm::value_ptr(color));

    // Create the transformation matrix
    glm::mat4 transform = glm::mat4(1.0f);

    float center_x = (min_x + max_x) / 2.0f;
    float center_y = (min_y + max_y) / 2.0f;

    float scale_x = (max_x - min_x);
    float scale_y = (max_y - min_y);

    transform = glm::translate(transform, glm::vec3(center_x * 2.0f - 1.0f, -(center_y * 2.0f - 1.0f), 0.0f));
    transform = glm::scale(transform, glm::vec3(scale_x * 2.0f, scale_y * 2.0f, 1.0f));

    int transform_loc = glGetUniformLocation(shader_ptr->shader_id, "transform");
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    mesh_ptr->unbind();
    shader_ptr->bind();
}

class UIElement : public std::enable_shared_from_this<UIElement>
{
    protected:
    bool active = true;
    float min_x, min_y, max_x, max_y;
    std::string name = "";
    std::shared_ptr<UIElement> parent;
    std::unordered_map<std::string, std::shared_ptr<UIElement>> children;

    public:
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

class UIButton : public UIElement
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
    UIButton(std::string name, float min_x, float min_y, float max_x, float max_y, const std::string& label, std::shared_ptr<lumina::Shader>& shader_ptr, std::shared_ptr<lumina::Font>& font_ptr, std::shared_ptr<lumina::Mesh>& mesh_ptr, glm::vec4& color, glm::vec4& text_color, glm::vec4& hover_color, glm::vec4& highlight_color)
    : UIElement(name, min_x, min_y, max_x, max_y), label(label), shader_ptr(shader_ptr), font_ptr(font_ptr), mesh_ptr(mesh_ptr), color(color), text_color(text_color), hover_color(hover_color), highlight_color(highlight_color)
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

        UIElement::render(window_width, window_height);
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
        UIElement::handleEvent(event, window_width, window_height);
    }

    void setOnClick(const std::function<void()>& callback)
    {
        on_click = callback;
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