#pragma once

#include <chrono>

namespace lumina
{
    namespace time
    {
        inline uint64_t getMilliseconds()
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
    }
}