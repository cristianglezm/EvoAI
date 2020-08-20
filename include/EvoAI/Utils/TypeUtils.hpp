#ifndef EVOAI_TYPE_UTILS_HPP
#define EVOAI_TYPE_UTILS_HPP

#include <memory>
#include <utility>
#include <string>
#include <JsonBox.h>
#include <type_traits>

#include <EvoAI/Export.hpp>

namespace estd{
    // Implementation from https://blog.tartanllama.xyz/detection-idiom/
    namespace detail{
        template<template<class...> class Trait, class Enabler, class... Args>
        struct is_detected : std::false_type{};

        template<template<class...> class Trait, class... Args>
        struct is_detected<Trait, std::void_t<Trait<Args...>>, Args...> : std::true_type{};
    }// namespace detail
    
    template<template<class...> class Trait, class... Args>
    using is_detected = typename detail::is_detected<Trait, void, Args...>::type;
}

namespace EvoAI{
   /**
    *  @brief T has a member function JsonBox::Value toJson() const noexcept
    */
    template<class T>
    using serializable_t = decltype(std::declval<T>().toJson());
    template <class T>
    constexpr bool serializable_v = estd::is_detected<serializable_t, T>::value;
    /**
     *  @brief T has a constructor T::T(JsonBox::Object)
     */
    template<class T>
    using serializable_constructible_t = decltype(T{std::declval<JsonBox::Object>()});
    template <class T>
    constexpr bool serializable_constructible_v = estd::is_detected<serializable_constructible_t, T>::value;
    /**
     *  @brief T has a member function std::size_t getID() const noexcept
     */  
    template<class T>
    using get_id_t = decltype(std::declval<T>().getID());
    template <class T>
    constexpr bool get_id_v = estd::is_detected<get_id_t, T>::value;
    /**
     *  @brief T has a member function void setID(const std::size_t&) noexcept
     */
    template<class T>
    using set_id_t = decltype(std::declval<T>().setID(std::declval<const std::size_t&>()));
    template <class T>
    constexpr bool set_id_v = estd::is_detected<set_id_t, T>::value;
    /**
     *  @brief T has a member function std::size_t getSpeciesID() const noexcept
     */  
    template<class T>
    using get_species_id_t = decltype(std::declval<T>().getSpeciesID());
    template <class T>
    constexpr bool get_species_id_v = estd::is_detected<get_species_id_t, T>::value;
    /**
     *  @brief T has a member function void setSpeciesID(const std::size_t&) noexcept
     */
    template<class T>
    using set_species_id_t = decltype(std::declval<T>().setSpeciesID(std::declval<const std::size_t&>()));
    template <class T>
    constexpr bool set_species_id_v = estd::is_detected<set_species_id_t, T>::value;
    /**
     *  @brief T has a member function void setFitness(const double&) noexcept
     */
    template<class T>
    using set_fitness_t = decltype(std::declval<T>().setFitness(std::declval<const double&>()));
    template <class T>
    constexpr bool set_fitness_v = estd::is_detected<set_fitness_t, T>::value;
    /**
     *  @brief T has a member function const double& getFitness() const noexcept
     */  
    template<class T>
    using get_fitness_t = decltype(std::declval<T>().getFitness());
    template <class T>
    constexpr bool get_fitness_v = estd::is_detected<get_fitness_t, T>::value;
    /**
     * @brief T has a static double T::distance(const T&, const T&, double, double, double) noexcept;
     */
    template<class T>
    using distance_t = decltype(T::distance(std::declval<const T&>(), std::declval<const T&>(), std::declval<const double&>(), std::declval<const double&>(), std::declval<const double&>()));
    template <class T>
    constexpr bool distance_v = estd::is_detected<distance_t, T>::value;
    /**
     * @brief T has a static T T::reproduce(const T&, const T&) noexcept;
     */
    template<class T>
    using reproduce_t = decltype(T::reproduce(std::declval<const T&>(), std::declval<const T&>()));
    template <class T>
    constexpr bool reproduce_v = estd::is_detected<reproduce_t, T>::value;
    /**
     *  @brief T requires to have these functions:
     *  
     *  T has a member function JsonBox::Value toJson() const noexcept
     *  T has a constructor T::T(JsonBox::Object)
     *  T has a member function const double& getFitness() const noexcept
     *  T has a member function void setFitness(const double&) noexcept
     */
    template<class T>
    struct is_speciable{
        static constexpr bool value = serializable_v<T> && serializable_constructible_v<T> &&
                                        get_fitness_v<T> && set_fitness_v<T>;
    };
    template<typename T>
    static constexpr bool is_speciable_v = is_speciable<T>::value;
    /**
     *  @brief T is required to have these functions:
     *  
     *  T has a member function JsonBox::Value toJson() const noexcept;
     *  T has a constructor T::T(JsonBox::Object);
     *  T has a member function const double& getFitness() const noexcept;
     *  T has a member function void setFitness(const double&) noexcept;
     *  T has a member function void setSpeciesID(const std::size_t&) noexcept;
     *  T has a member function std::size_t getSpeciesID() const noexcept;
     *  T has a member function void setID(const std::size_t&) noexcept;
     *  T has a member function std::size_t getID() const noexcept;
     *  T has a static double T::distance(const T&, const T&, double, double, double) noexcept;
     *  T has a static T T::reproduce(const T&, const T&) noexcept;
     */
    template<class T>
    struct is_populable{
        static constexpr bool value = is_speciable<T>::value && get_species_id_v<T> && get_id_v<T> &&
                                        set_species_id_v<T> && set_id_v<T> && distance_v<T> && reproduce_v<T>;
    };
    template<typename T>
    static constexpr bool is_populable_v = is_populable<T>::value;
    /** @TODO use this when c++20
    template<class T>
    concept is_speciable = 
        requires{T{std::declval<JsonBox::Object>()};} &&
        requires{std::declval<const T>().toJson();} &&
        requires{std::declval<const T>().getFitness();} &&
        requires{std::declval<T>().setFitness(std::declval<const std::size_t&>());};

    template<class T>
    concept is_populable = 
        requires{is_speciable<T>;} &&
        requires{std::declval<const T>().getID();} &&
        requires{std::declval<T>().setID(std::declval<const std::size_t&>());} &&
        requires{std::declval<const T>().getSpeciesID();} &&
        requires{std::declval<T>().setSpeciesID(std::declval<const std::size_t&>());} &&
        requires{T::distance(std::declval<const T>(), std::declval<const T>(), std::declval<const double&>(), std::declval<const double&>(), std::declval<const double&>());} &&
        requires{T::reproduce(std::declval<const T>(), std::declval<const T>());};
    */
}

#endif // EVOAI_TYPE_UTILS_HPP