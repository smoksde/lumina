#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <GL/glew.h>

namespace lumina
{
    class Viewport
    {   
        public:
        int frame_width;
        int frame_height;

        SDL_Window* window;
        SDL_GLContext gl_context;

        Viewport(const char* window_title, int frame_width, int frame_height)
        {
            if (SDL_Init(SDL_INIT_VIDEO) < 0)
            {
                std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
                return;
            }

            window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, frame_width, frame_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
            if (!window)
            {
                std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
                SDL_Quit();
                return;
            }

            gl_context = SDL_GL_CreateContext(window);
            if (!gl_context)
            {
                std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
                SDL_DestroyWindow(window);
                SDL_Quit();
                return;
            }

            if (SDL_GL_MakeCurrent(window, gl_context) != 0)
            {
                std::cerr << "Failed to make OpenGL context current: " << SDL_GetError() << std::endl;
                SDL_GL_DeleteContext(gl_context);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return;
            }

            static bool glew_initialized = false;
            if (!glew_initialized)
            {
                GLenum glewError = glewInit();
                if (glewError != GLEW_OK)
                {
                    std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(glewError) << std::endl;
                    SDL_GL_DeleteContext(gl_context);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return;
                }
                glew_initialized = true;
            }

            std::cout << "Viewport initialized: " << window_title << std::endl;
        }

        ~Viewport()
        {
            if (gl_context)
            {
                SDL_GL_DeleteContext(gl_context);
            }
            if (window)
            {
                SDL_DestroyWindow(window);
            }
            SDL_Quit();
        }

        void makeCurrent()
        {
            SDL_GL_MakeCurrent(window, gl_context);
        }

        void swapBuffers()
        {
            SDL_GL_SwapWindow(window);
        }
    };
}
