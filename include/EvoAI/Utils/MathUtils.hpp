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
    static constexpr double PI = 3.14159265358979323846;
    /**
     * @brief Combines two hashes
     * @tparam T number type
     * @param seed std::size_t&
     * @param v const T& hasheable Object
     */
    template<class T>
    inline void hashCombine(std::size_t& seed, const T& v) noexcept;
    /**
     *  @brief joins hashes 
     *  @code
     *      auto seed = 42u;
     *      auto hash = EvoAI::joinHashes(seed, 1,2,3,4,5);
     *  @endcode
     *  @tparam ...Args seq of numbers(same type)
     *  @param [in] seed to use
     *  @param [in] args number seq
     *  @return std::size_t hash
     */
    template<typename...Args>
    inline constexpr std::size_t joinHashes(std::size_t seed, const Args&...args) noexcept;
    /**
     * @brief Scales a number to be between normalized min and normalized max.
     * @tparam T type
     */
    template<typename T>
    struct EvoAI_API Scaler{
        using type = T;
        using value_type = std::remove_reference_t<std::remove_pointer_t<T>>;
        using reference = value_type&;
        using rvalue_reference = value_type&&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        static_assert(std::is_arithmetic_v<value_type>, "T must be an arithmetic number.");
        /**
         * @brief Constructor scaled values to be (default: 0.0, 1.0)
         * @param normalizedMin value_type
         * @param normalizedMax value_type
         */
        Scaler(value_type normalizedMin = 0.0, value_type normalizedMax = 1.0);
        /**
         * @brief Constructor for a JsonBox::Object.
         * @param o JsonBox::Object
         */
        Scaler(JsonBox::Object o);
        /**
         * @brief Use this to get a JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief Detects and saves min and max from the data and applies the transform to 
         *  scale the values to be within normMin and normMax
         * @code
         *      std::vector<double> data = {10.0, 2.0, 3.0, 1.0, 5.0}
         *      EvoAI::Scaler<double> sc(0.0, 1.0);
         *      sc.tranform(data);
         * @endcode
         * @param data std::vector<type>& transforms the data
         */
        template<typename U>
        void transform(std::vector<U>& data) noexcept;
        /**
         * @brief Detects and saves min and max from the data and applies the transform to 
         *  scale the range to be within normMin and normMax
         * @code
         *      std::vector<double> data = {10.0, 2.0, 3.0, 1.0, 5.0}
         *      EvoAI::Scaler<double> sc(0.0, 1.0);
         *      sc.tranform(std::begin(data),std::end(data));
         * @endcode
         * @param begin Iter
         * @param end Iter
         */
        template<typename Iter>
        void transform(Iter begin, Iter end) noexcept;
        /**
         * @brief Applies transform to a single value, set the min and 
         *          max by hand or use transform(std::vector<T>&) first.
         * @warning set .min and .max before calling this method.
         * @code
         *      EvoAI::Scaler<double> sc(0.0, 1.0); 
         *      sc.min = 0.0; 
         *      sc.max = 100.0;
         *      auto newData = sc.tranform(5.42);
         * @endcode
         * @param val
         * @return T new value
         */
        value_type transform(value_type val) noexcept;
        /**
         * @brief converts the data to the unscaled data.
         * @code
         *      std::vector<double> data = {10.0, 2.0, 3.0, 1.0, 5.0}
         *      EvoAI::Scaler<double> sc(0.0, 1.0);
         *      sc.inverseTranform(data);
         * @endcode
         * @param data std::vector<type> modifies the data
         */
        template<typename U>
        void inverseTransform(std::vector<U>& data) noexcept;
        /**
         * @brief converts the data in the range to the unscaled data.
         * @code
         *      std::vector<double> data = {10.0, 2.0, 3.0, 1.0, 5.0}
         *      EvoAI::Scaler<double> sc(0.0, 1.0);
         *      sc.inverseTranform(std::begin(data),std::end(data));
         * @endcode
         * @param begin Iter
         * @param end Iter
         */
        template<typename Iter>
        void inverseTransform(Iter begin, Iter end) noexcept;
        /**
         * @brief converts a single value, set the min and max by hand or
         *        use transform(std::vector<T>&) or transform(Iter begin, Iter end) first
         * @warning set .min and .max 
         * @code
         *      EvoAI::Scaler<double> sc(0.0, 1.0); 
         *      sc.min = 0.0; 
         *      sc.max = 100.0;
         *      auto newData = sc.inverseTranform(5.42);
         * @endcode
         * @param val
         * @return T new value
         */
        value_type inverseTransform(value_type val) noexcept;
        ~Scaler() = default;
        value_type normMin;
        value_type normMax;
        value_type min;
        value_type max;
    };
    /**
     * @brief normalize values from one range to another
     * @code
     *  normalize<float>(50,-1.0,1.0,0.0,100.0);
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
     *  normalize<float>(50,0.0,100.0);
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
     * @param x1 const T&
     * @param y1 const T&
     * @param x2 const T&
     * @param y2 const T&
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceManhattan(const T& x1, const T& y1, const T& x2, const T& y2) noexcept;
    /**
     * @brief Calculates the distance using Chebyshev between x1,y1 and x2, y2
     * @param x1 const T&
     * @param y1 const T&
     * @param x2 const T&
     * @param y2 const T&
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceChebyshev(const T& x1, const T& y1, const T& x2, const T& y2) noexcept;
    /**
     * @brief Calculates the distance using Euclidean between x1,y1 and x2, y2
     * @param x1 const T&
     * @param y1 const T&
     * @param x2 const T&
     * @param y2 const T&
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceEuclidean(const T& x1, const T& y1, const T& x2, const T& y2) noexcept;
    /**
     * @brief Calculates the distance to Center
     * @param x const T&
     * @param y const T&
     * @param width const T&
     * @param height const T&
     * @return T 
     */
    template<typename T>
    inline constexpr T distanceCenter(const T& x, const T& y, const T& width, const T& height) noexcept;
//////////
///// implementation inlined functions.
//////////
    template <class T>
    inline void hashCombine(std::size_t& seed, const T& v) noexcept{
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    template<typename...Args>
    inline constexpr std::size_t joinHashes(std::size_t seed, const Args&...args) noexcept{
        for(const auto& n:{args...}){
            hashCombine(seed, n);
        }
        return seed;
    }
    template<typename T>
    Scaler<T>::Scaler(Scaler<T>::value_type normalizedMin, Scaler<T>::value_type normalizedMax)
    : normMin(normalizedMin)
    , normMax(normalizedMax)
    , min(0.0)
    , max(0.0){}
    template<typename T>
    Scaler<T>::Scaler(JsonBox::Object o)
    : normMin(o["normMin"].getFloat())
    , normMax(o["normMax"].getFloat())
    , min(o["min"].getFloat())
    , max(o["max"].getFloat()){}
    template<typename T>
    JsonBox::Value Scaler<T>::toJson() const noexcept{
        JsonBox::Object o;
        o["normMin"] = normMin;
        o["normMax"] = normMax;
        o["min"] = min;
        o["max"] = max;
        return o;
    }
    template<typename T>
    template<typename U>
    void Scaler<T>::transform(std::vector<U>& data) noexcept{
        transform(std::begin(data), std::end(data));
    }
    template<typename T>
    template<typename Iter>
    void Scaler<T>::transform(Iter begin, Iter end) noexcept{
        if constexpr(std::is_pointer_v<typename Iter::value_type>){
            auto minmax = std::minmax_element(begin, end, 
                [](auto& lhs, auto& rhs){
                    return *lhs < *rhs;
                });
            min = *(*minmax.first);
            max = *(*minmax.second);
            std::for_each(begin, end, 
                [&](auto& d){
                    *d = transform(*d);
                });
        }else{
            auto minmax = std::minmax_element(begin, end);
            min = *minmax.first;
            max = *minmax.second;
            std::for_each(begin, end, 
                [&](auto& d){
                    d = transform(d);
                });
        }
    }
    template<typename T>
    inline typename Scaler<T>::value_type Scaler<T>::transform(Scaler<T>::value_type val) noexcept{
        return (normMin + (val-min)*(normMax-normMin)/(max-min));
    }
    template<typename T>
    template<typename U>
    void Scaler<T>::inverseTransform(std::vector<U>& data) noexcept{
        inverseTransform(std::begin(data), std::end(data));
    }
    template<typename T>
    template<typename Iter>
    void Scaler<T>::inverseTransform(Iter begin, Iter end) noexcept{
        if constexpr(std::is_pointer_v<typename Iter::value_type>){
            std::for_each(begin, end, 
                [&](auto d){
                    *d = (*d) * max + min;
            });
        }else{
            std::for_each(begin, end, 
                [&](auto& d){
                    d = d * max + min;
            });
        }
    }
    template<typename T>
    inline typename Scaler<T>::value_type Scaler<T>::inverseTransform(Scaler<T>::value_type val) noexcept{
        return val * max + min;
    }
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
}

#endif // EVOAI_MATH_UTILS_HPP