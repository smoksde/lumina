#pragma once

#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtx/norm.hpp"
#include "../libs/glm/gtc/quaternion.hpp"
#include "../libs/glm/ext/matrix_transform.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"
#include "../libs/glm/gtx/string_cast.hpp"

#include "../libs/json.hpp"

#include "viewport.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include "camera.hpp"
#include "node.hpp"
#include "mesh.hpp"
#include "font.hpp"
#include "terminal.hpp"
#include "object.hpp"
#include "geometry.hpp"
#include "texture.hpp"
#include "ui/ui.hpp"
#include "loader.hpp"
#include "time/time.hpp"
#include "time/timer.hpp"

namespace lumina
{
    inline SDL_Window* createWindow(const char* title, int width, int height, int x, int y) {
        SDL_Window* window = SDL_CreateWindow(title, x, y, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (!window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
            SDL_Quit();
            exit(1);
        }
        return window;
    }

    inline int initSDL(SDL_Window*& window, SDL_GLContext& gl_context)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            exit(1);
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_SetWindowFullscreen(window, 0);
        
        gl_context = SDL_GL_CreateContext(window);
        if (!gl_context)
        {
            std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(window);
            SDL_Quit();
            return -1;
        }

        SDL_GL_MakeCurrent(window, gl_context);

        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW\n";
            SDL_GL_DeleteContext(gl_context);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return -1;
        }
        return 0;
    }
}