#ifndef EVOAI_UTILS_HPP
#define EVOAI_UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>
#include <functional>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

#include <EvoAI/Utils/MathUtils.hpp>
#include <EvoAI/Utils/NNUtils.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>

namespace EvoAI{
    /**
     *  @brief calculates time it takes Fn in milliseconds
     *  
     *  @param [in] fn   function
     *  @param [in] args args for function
     *  @return double
     */
    template<typename Fn, typename...Args>
    auto TimeThis(Fn&& fn, const Args&...args) noexcept{
        auto start = std::chrono::high_resolution_clock::now();
        std::invoke(fn, args...);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end-start).count();
    }
}

#if defined ANDROID && USE_MAKE_UNIQUE
    namespace std{
        /**
         * @brief Utility to create a std::unique_ptr
         * @tparam T 
         * @tparam args Args arguments
         * @return std::unique_ptr<T<Args>>
         */
        template<typename T, typename... Args>
        unique_ptr<T> make_unique(Args&&... args){
            return unique_ptr<T>(new T(forward<Args>(args)...));
        }
    }
#endif
#endif // EVOAI_UTILS_HPP