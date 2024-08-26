#pragma once

#include "lumina.hpp"

class Loop
{
    public:

    bool running;

    int fps;
    int tps;

    Loop(int fps, int tps = 1)
    : fps(fps), tps(tps)
    {

    }

    void run()
    {
        uint64 sdl_time = SDL_GetTicks64();
        uint64 last_tick_time = sdl_time;
        uint64 last_render_time = sdl_time;

        while(running)
        {
            sdl_time = SDL_GetTicks64();
        }
    }

    virtual void handle()
    {
        
    }

    virtual void update()
    {

    }

    virtual void render()
    {

    }
};