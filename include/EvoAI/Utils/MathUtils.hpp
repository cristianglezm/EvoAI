#ifndef EVOAI_MATH_UTILS_HPP
#define EVOAI_MATH_UTILS_HPP

#include <memory>
#include <utility>
#include <random>
#include <chrono>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     * @brief Combines two hashes
     * @param seed std::size_t
     * @param v const T& hasheable Object
     * @return std::size_t
     */
    template <class T>
    inline std::size_t hashCombine(std::size_t seed, const T& v) noexcept;
    /**
     * @brief normalize values from one range to another
     * @code
     * normalize<float>(50,-1.0,1.0,0.0,100.0);
     * @endcode
     * @tparam number type
     * @param val T
     * @param normMin T
     * @param normMax T
     * @param min T
     * @param max T
     * @return T
     */
    template<typename T = float>
    inline constexpr T normalize(const T& val, const T& normMin, const T& normMax, const T& min, const T& max) noexcept;
    /**
     * @brief normalize values from one range to another 
     * @code
     * normalize<float>(50,0.0,100.0);
     * @endcode
     * @tparam number type
     * @param val T
     * @param min T
     * @param max T
     * @return T
     */
    template<typename T = float>
    inline constexpr T stableNormalize(const T& val, const T& min, const T& max) noexcept;
    /**
     * @brief Calculates the distance using Manhattan between x1,y1 and x2, y2
     * @param const T& x1
     * @param const T& y1
     * @param const T& x2
     * @param const T& y2
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceManhattan(const T& x1, const T& y1, const T& x2, const T& y2) noexcept;
    /**
     * @brief Calculates the distance using Chebyshev between x1,y1 and x2, y2
     * @param const T& x1
     * @param const T& y1
     * @param const T& x2
     * @param const T& y2
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceChebyshev(const T& x1, const T& y1, const T& x2, const T& y2) noexcept;
    /**
     * @brief Calculates the distance using Euclidean between x1,y1 and x2, y2
     * @param const T& x1
     * @param const T& y1
     * @param const T& x2
     * @param const T& y2
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceEuclidean(const T& x1, const T& y1, const T& x2, const T& y2) noexcept;
    /**
     * @brief Calculates the distance to Center
     * @param const T& x
     * @param const T& y
     * @param const T& width
     * @param const T& height
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceCenter(const T& x, const T& y, const T& width, const T& height) noexcept;
//////////
///// implementation inlined functions.
//////////
    template<typename T>
    inline constexpr T normalize(const T& val,const T& normMin, const T& normMax, const T& min, const T& max) noexcept{
        if(min<max){
            return (normMin + (val-min)*(normMax-normMin)/(max-min));
        }
        return val;
    }
    template<typename T>
    inline constexpr T stableNormalize(const T& val, const T& min, const T& max) noexcept{
        if(min<max){
            return ((val - min) / (max - min)); // avoid nan values
        }
        return val;
    }
    template<typename T>
    inline constexpr T distanceManhattan(const T& x1, const T& y1, const T& x2, const T& y2) noexcept{
        return (10 *(std::abs(x1-x2) + std::abs(y1-y2)));
    }
    template<typename T>
    inline constexpr T distanceChebyshev(const T& x1, const T& y1, const T& x2, const T& y2) noexcept{
        return std::max(x1 - x2, y1 - y2);
    }
    template<typename T>
    inline constexpr T distanceEuclidean(const T& x1, const T& y1, const T& x2, const T& y2) noexcept{
        return std::hypot(x1 - x2, y1 - y2);
    }
    template<typename T>
    inline constexpr T distanceCenter(const T& x, const T& y, const T& width, const T& height) noexcept{
        auto normX = (2.0*(x/width))-1;
        auto normY = (2.0*(y/height))-1;
        return std::sqrt((static_cast<int>(normX/2)^2) + (static_cast<int>(normY/2)^2));
    }
    template <class T>
    inline std::size_t hashCombine(std::size_t seed, const T& v) noexcept{
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        return seed;
    }
}

#endif // EVOAI_MATH_UTILS_HPP