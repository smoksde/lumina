#pragma once

#include <chrono>

namespace lumina {
namespace time {

class Timer
{
private:
    using clock = std::chrono::high_resolution_clock;
    clock::time_point frame_start_time;
    clock::time_point last_timestamp;

    float delta_time = 0.0f;
    float accumulator = 0.0f;
    float fps = 0.0f;
    int frame_count = 0;
    float frame_accumulator = 0.0f;
    const float tick_intervall = 1.0f;
    bool tick_due = false;

public:
    inline Timer()
    {
        last_timestamp = clock::now();
    }

    inline void startFrame()
    {
        frame_start_time = clock::now();
    }

    inline void endFrame()
    {
        auto frame_end_time = clock::now();
        std::chrono::duration<float> frame_duration =
            frame_end_time - frame_start_time;
        (void)frame_duration; // suppress unused warning
    }

    inline void update()
    {
        auto now = clock::now();
        std::chrono::duration<float> delta = now - last_timestamp;
        delta_time = delta.count();
        last_timestamp = now;

        frame_accumulator += delta_time;
        accumulator += delta_time;
        frame_count++;

        if (frame_accumulator >= 1.0f)
        {
            fps = frame_count / frame_accumulator;
            frame_accumulator = 0.0f;
            frame_count = 0;
        }

        if (accumulator >= tick_intervall)
        {
            tick_due = true;
        }
    }

    inline float getDeltaTime() const
    {
        return delta_time;
    }

    inline float getElapse() const
    {
        return accumulator / tick_intervall;
    }

    inline float getFPS() const
    {
        return fps;
    }

    inline bool shouldTick()
    {
        return tick_due;
    }

    inline void resetTick()
    {
        accumulator = 0.0f;
        tick_due = false;
    }
};


}
}
