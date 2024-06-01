#ifndef EVOAI_TIMER_HPP
#define EVOAI_TIMER_HPP

#include <functional>
#include <chrono>

namespace EvoAI{
    /**
     * @brief a timer
     */
    class Timer final{
        public:
            using Clock = std::chrono::steady_clock;
        public:
            /**
             * @brief constructor
             * @param duration std::chrono::duration<std::int64_t, std::milli> duration in milliseconds
             */
            Timer(std::chrono::duration<std::int64_t, std::milli> duration) noexcept;
            /**
             * @brief checks if the duration has been exceeded
             * @code
             *  Timer(std::chrono::seconds(20));
             *  while(true){
             *      if(timer.update()){
             *          // code to exec every 20 secods
             *      }
             *  }
             * @endcode
             * @return bool true if mStart > mDuration
             */
            bool update() noexcept;
        private:
            Clock::time_point mStart;
            Clock::time_point mDuration;
    };   
}

#endif // EVOAI_TIMER_HPP