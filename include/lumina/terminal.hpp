#pragma once

#include "../libs/glm/glm.hpp"
#include "../libs/glm/ext/matrix_transform.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"

#include <string>
#include <memory>

#include "font.hpp"
#include "viewport.hpp"
#include "utils.hpp"

namespace lumina
{
    class Terminal
    {
        public:

        bool selected = false;

        std::shared_ptr<Font> font_ptr;
        // std::shared_ptr<Viewport> viewport_ptr;
        SDL_Window* window;
        
        std::string terminal_prefix = "TERMINAL > ";
        std::string terminal_input = "";
        glm::vec3 terminal_string_color = glm::vec3(0.4f, 0.4f, 0.4f);
        glm::vec3 weak_color = glm::vec3(0.4f, 0.4f, 0.4f);
        glm::vec3 highlight_color = glm::vec3(1.0f, 1.0f, 1.0f);

        Terminal(SDL_Window* window, std::shared_ptr<Font> font_ptr)
        : window(window), font_ptr(font_ptr)
        {
            
        }

        void render()
        {
            font_ptr->shader_ptr->bind();
            glUniform4f(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_color"), terminal_string_color.x, terminal_string_color.y, terminal_string_color.z, 1.0f);
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            glm::mat4 ortho = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
            glUniformMatrix4fv(glGetUniformLocation(font_ptr->shader_ptr->shader_id, "u_modelViewProj"), 1, GL_FALSE, glm::value_ptr(ortho));
            int x, y;
            int padding_bottom = 20;
            int padding_left = 10;
            relative2Pixel(0.0f, 1.0f, w, h, x, y);
            std::string terminal_render_string = "";
            terminal_render_string.append(terminal_prefix);
            terminal_render_string.append(terminal_input);
            font_ptr->drawString(float(x+padding_left), float(y-padding_bottom), terminal_render_string.c_str()); 
            font_ptr->shader_ptr->unbind();
        }

        void select()
        {
            terminal_string_color = highlight_color;
            selected = true;
        }

        void deselect()
        {
            terminal_string_color = weak_color;
            selected = false;
        }

        void processInput(SDL_Event& event)
        {
            if (event.type == SDL_KEYDOWN && selected) {
                SDL_Keycode key = event.key.keysym.sym;

                if (key == SDLK_BACKSPACE && !terminal_input.empty()) {
                    terminal_input.pop_back();
                } else if (key == SDLK_RETURN) {
                    // Handle command submission (e.g., execute or log the input)
                    // submitCommand();
                    terminal_input.clear();
                }
            } else if (event.type == SDL_TEXTINPUT && selected) {
                terminal_input.append(event.text.text);
            }
        }
        private:
    };
}
