#pragma once

#include <string>
namespace lumina::ui {

enum class TextAlign {
    Center,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
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



inline void drawText(const ::std::string& text, const glm::vec4& bounds, lumina::Font& font, glm::vec4& color, int with, int height, lumina::ui::TextAlign align = lumina::ui::TextAlign::Center)
{
    glm::vec2 text_size = font.measureString(text.c_str());

    float text_x = 0.0f;
    float text_y = 0.0f;

    switch (align)
    {
        case lumina::ui::TextAlign::Left:
            text_x = bounds.x * with;
            text_y = bounds.y * height + (text_size.y);
            break;
        case lumina::ui::TextAlign::Center:
            text_x = ((bounds.x + bounds.z) / 2.0f) * with - (text_size.x / 2.0f);
            text_y = ((bounds.y + bounds.w) / 2.0f) * height + (text_size.y / 2.0f);
            break;
        case lumina::ui::TextAlign::Right:
            text_x = bounds.z * with - text_size.x;
            text_y = bounds.y * height + (text_size.y);
            break;
        default:
            text_x = bounds.x * with;
            text_y = bounds.y * height + (text_size.y);
            break;
    }

    font.shader_ptr->bind();

    glUniform4f(glGetUniformLocation(font.shader_ptr->shader_id, "u_color"), color.r, color.g, color.b, color.a);
    glm::mat4 ortho = glm::ortho(0.0f, (float)with, (float)height, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(font.shader_ptr->shader_id, "u_modelViewProj"), 1, GL_FALSE, glm::value_ptr(ortho));
    font.drawString(text_x, text_y, text.c_str(), color);
    font.shader_ptr->unbind();


}

}