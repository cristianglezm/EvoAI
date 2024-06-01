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

namespace EvoAI::meta{
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
     *  @brief T has a member function void setID(std::size_t) noexcept
     */
    template<class T>
    using set_id_t = decltype(std::declval<T>().setID(std::declval<std::size_t>()));
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
     *  @brief T has a member function void setSpeciesID(std::size_t) noexcept
     */
    template<class T>
    using set_species_id_t = decltype(std::declval<T>().setSpeciesID(std::declval<std::size_t>()));
    template <class T>
    constexpr bool set_species_id_v = estd::is_detected<set_species_id_t, T>::value;
    /**
     *  @brief T has a member function void setFitness(double) noexcept
     */
    template<class T>
    using set_fitness_t = decltype(std::declval<T>().setFitness(std::declval<double>()));
    template <class T>
    constexpr bool set_fitness_v = estd::is_detected<set_fitness_t, T>::value;
    /**
     *  @brief T has a member function double getFitness() const noexcept
     */ 
    template<class T>
    using get_fitness_t = decltype(std::declval<T>().getFitness());
    template <class T>
    constexpr bool get_fitness_v = estd::is_detected<get_fitness_t, T>::value;
    /**
     * @brief T has a static double T::distance(const T&, const T&, double, double, double) noexcept;
     */
    template<class T>
    using distance_t = decltype(T::distance(std::declval<const T&>(), std::declval<const T&>(), std::declval<double>(), std::declval<double>(), std::declval<double>()));
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
     *  @details
     *  T has a member function JsonBox::Value toJson() const noexcept;
     *  T has a constructor T::T(JsonBox::Object);
     *  T has a member function double getFitness() const noexcept;
     *  T has a member function void setFitness(double) noexcept;
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
     *  @details
     *  T has a member function JsonBox::Value toJson() const noexcept;
     *  T has a constructor T::T(JsonBox::Object);
     *  T has a member function double getFitness() const noexcept;
     *  T has a member function void setFitness(double) noexcept;
     *  T has a member function void setSpeciesID(std::size_t) noexcept;
     *  T has a member function std::size_t getSpeciesID() const noexcept;
     *  T has a member function void setID(std::size_t) noexcept;
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
    /// @todo use this when c++20
    /*
    template<class T>
    concept is_speciable = 
        requires{T{std::declval<JsonBox::Object>()};} &&
        requires{std::declval<const T>().toJson();} &&
        requires{std::declval<const T>().getFitness();} &&
        requires{std::declval<T>().setFitness(std::declval<std::size_t>());};

    template<class T>
    concept is_populable = 
        requires{is_speciable<T>;} &&
        requires{std::declval<const T>().getID();} &&
        requires{std::declval<T>().setID(std::declval<std::size_t>());} &&
        requires{std::declval<const T>().getSpeciesID();} &&
        requires{std::declval<T>().setSpeciesID(std::declval<std::size_t>());} &&
        requires{T::distance(std::declval<const T>(), std::declval<const T>(), std::declval<double>(), std::declval<double>(), std::declval<double>());} &&
        requires{T::reproduce(std::declval<const T>(), std::declval<const T>());};
    */
    /**
     *  @brief T has a constructor T::T(JsonBox::Object, C)
     *  @tparam T Class or struct with constructor T::T(JsonBox::Object, C);
     *  @tparam C additional parameter for the constructor
     */
    template<class T, class C>
    using serializable_constructible_with_param_t = decltype(T{std::declval<JsonBox::Object>(), std::declval<C>()});
    template <class T, class C>
    constexpr bool serializable_constructible_with_param_v = estd::is_detected<serializable_constructible_with_param_t, T, C>::value;
    /**
     * @brief T has a void T::operator()(double, std::size_t) const noexcept;
     */
    template<class T>
    using optimizer_algo_t = decltype(std::declval<T>().operator()(std::declval<double>(), std::declval<std::size_t>()));
    template <class T>
    constexpr bool optimizer_algo_v = estd::is_detected<optimizer_algo_t, T>::value;
    /**
     * @brief T is required to have these functions:
     * @details
     *  T has a constructor T::T(JsonBox::Object, NeuralNetwork*);
     *  T has a member function JsonBox::Value toJson() const noexcept;
     *  T has a T::operator()(double, std::size_t) const noexcept;
     */
    template<class T, class C>
    struct is_an_optimizer_algorithm{
        static constexpr bool value = serializable_v<T> && serializable_constructible_with_param_v<T, C> && optimizer_algo_v<T>;
    };
    template<class T, class C>
    static constexpr bool is_an_optimizer_algorithm_v = is_an_optimizer_algorithm<T,C>::value;
    /**
     * @brief T has a double T::operator()(double, std::size_t) const noexcept;
     */
    template<class T>
    using scheduler_algo_t = decltype(std::declval<T>().operator()(std::declval<double>(), std::declval<std::size_t>()));
    template<class T>
    constexpr bool scheduler_algo_v = estd::is_detected<scheduler_algo_t, T>::value;
    /**
     * @brief T is required to have these functions:
     * @details
     *  T has a constructor T::T(JsonBox::Object);
     *  T has a member function JsonBox::Value toJson() const noexcept;
     *  T has a T::operator()(double, std::size_t) const noexcept;
     */
    template<class T>
    struct is_a_scheduler_algorithm{
        static constexpr bool value = serializable_v<T> && serializable_constructible_v<T> && scheduler_algo_v<T>;
    };
    template<class T>
    static constexpr bool is_a_scheduler_algorithm_v = is_a_scheduler_algorithm<T>::value;
   /**
     *  @brief T has a member function double operator()(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept
     */
    template<class T>
    using has_forward_t = decltype(std::declval<T>().operator()(std::declval<const std::vector<double>&>(), std::declval<const std::vector<double>&>()));
    template<class T>
    static constexpr bool has_forward_v = estd::is_detected<has_forward_t, T>::value;
   /**
     *  @brief T has a member function std::vector<double> backward(const std::vector<double>& expectedOutputs, const std::vector<double>& outputs) noexcept
     */
    template<class T>
    using has_backward_t = decltype(std::declval<T>().backward(std::declval<const std::vector<double>&>(), std::declval<const std::vector<double>&>()));
    template<class T>
    static constexpr bool has_backward_v = estd::is_detected<has_backward_t, T>::value;
    /**
     * @brief T is a loss algorithm.
     */
    template<class T>
    static constexpr bool is_a_loss_v = has_forward_v<T> && has_backward_v<T>;
   /**
     *  @brief T has a member function std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept
     */
    template<class T>
    using has_empty_operator_t = decltype(std::declval<T>().operator()());
    template<class T>
    static constexpr bool has_empty_operator_v = estd::is_detected<has_empty_operator_t, T>::value;
   /**
     *  @brief T has a member function std::size_t size() noexcept
     */
    template<class T>
    using has_size_t = decltype(std::declval<T>().size());
    template<class T>
    static constexpr bool has_size_v = estd::is_detected<has_size_t, T>::value;
   /**
     *  @brief T has a member function std::size_t getBatchSize() noexcept
     */
    template<class T>
    using has_get_batch_size_t = decltype(std::declval<T>().getBatchSize());
    template<class T>
    static constexpr bool has_get_batch_size_v = estd::is_detected<has_get_batch_size_t, T>::value;
   /**
     *  @brief T has a member function void shuffle() noexcept
     */
    template<class T>
    using has_shuffle_t = decltype(std::declval<T>().shuffle());
    template<class T>
    static constexpr bool has_shuffle_v = estd::is_detected<has_shuffle_t, T>::value;
    template<class T>
    static constexpr bool is_a_dataset_v = has_empty_operator_v<T> && has_size_v<T> && has_get_batch_size_v<T> && has_shuffle_v<T>;
}

#endif // EVOAI_TYPE_UTILS_HPP
