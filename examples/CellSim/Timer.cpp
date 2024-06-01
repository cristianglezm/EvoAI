#include "Timer.hpp"

namespace EvoAI{
    Timer::Timer(std::chrono::duration<std::int64_t, std::milli> duration) noexcept
    : mStart(Clock::now())
    , mDuration(duration){}
    bool Timer::update() noexcept{
        auto end = Clock::now();
        bool ringed = (end - mStart).count() >= mDuration.time_since_epoch().count();
        if(ringed){
            mStart = Clock::now();
        }
        return ringed; 
    }
}