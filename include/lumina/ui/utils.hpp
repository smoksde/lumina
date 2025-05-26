#pragma once

namespace lumina {

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

}