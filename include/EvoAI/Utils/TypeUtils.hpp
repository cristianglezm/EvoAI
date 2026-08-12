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
     * @brief parses str as an unsigned integer, returning defaultValue instead
     * of throwing when str is empty or not a valid number.
     * @tparam T an unsigned integral type, defaults to std::size_t
     * @param str the string to parse (e.g. from JsonBox::Value::getString())
     * @param defaultValue value returned when str can't be parsed
     * @return T
     */
    template<typename T = std::size_t>
    [[nodiscard]] T safeParseUInt(const std::string& str, T defaultValue = T{0}) noexcept{
        if(str.empty()){
            return defaultValue;
        }
        try{
            return static_cast<T>(std::stoull(str));
        } catch(const std::exception&){
            return defaultValue;
        }
    }
}// namespace EvoAI

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
    /**
     * @brief Node has .index
     */
    template<class T>
    using node_index_t = decltype(std::declval<T>().index);
    /**
     * @brief Node has .x
     */
    template<class T>
    using has_x_t = decltype(std::declval<T>().x);
    template<class T>
    constexpr bool has_x_v = estd::is_detected<has_x_t, T>::value;
    /**
     * @brief Node has .y
     */
    template<class T>
    using has_y_t = decltype(std::declval<T>().y);
    template<class T>
    constexpr bool has_y_v = estd::is_detected<has_y_t, T>::value;
    /**
     * @brief Node has .blocked
     */
    template<class T>
    using has_blocked_t = decltype(std::declval<T>().blocked);
    template<class T>
    constexpr bool has_blocked_v = estd::is_detected<has_blocked_t, T>::value;
    /**
	 * @brief A node for Graph must have these requirements:
	 * @details
	 *		T has a .index
	 *      T is json serializable
	 *  	T is json deserializable
     */
    template<class T>
    constexpr bool is_a_graph_node_v =
      serializable_constructible_v<T> &&
      serializable_v<T> &&
      estd::is_detected<node_index_t, T>::value;
    /**
     * @brief Edge has .prev
     */
    template<class T>
    using edge_prev_t = decltype(std::declval<T>().prev);
	    /**
     * @brief Edge has .next
     */
    template<class T>
    using edge_next_t = decltype(std::declval<T>().next);
    /**
     * @brief Edge has .weight
     */
    template<class T>
    using edge_weight_t = decltype(std::declval<T>().weight);
	/**
	 * @brief An edge for Graph must have these requirements:
	 * @details
	 *		T has a .prev
	 *		T has a .next
	 *		T has a .weight
	 *      T is json serializable
	 *  	T is json deserializable
	 */
    template<class T>
    constexpr bool is_a_graph_edge_v =
      serializable_v<T> &&
      serializable_constructible_v<T> &&
      estd::is_detected<edge_prev_t, T>::value &&
      estd::is_detected<edge_next_t, T>::value &&
      estd::is_detected<edge_weight_t, T>::value;
    /**
     * @brief T has a member function bool canTraverse(const NodeType&, const EdgeType&, const NodeType&) noexcept
     */
    template<class T, class NodeType, class EdgeType>
    using has_can_traverse_t = decltype(std::declval<T>().canTraverse(std::declval<const NodeType&>(), std::declval<const EdgeType&>(), std::declval<const NodeType&>()));
    template<class T, class NodeType, class EdgeType>
    static constexpr bool has_can_traverse_v = estd::is_detected<has_can_traverse_t, T, NodeType, EdgeType>::value;
    /**
     * @brief T has a member function double cost(const NodeType&, const EdgeType&, const NodeType&) noexcept
     */
    template<class T, class NodeType, class EdgeType>
    using has_cost_t = decltype(std::declval<T>().cost(std::declval<const NodeType&>(), std::declval<const EdgeType&>(), std::declval<const NodeType&>()));
    template<class T, class NodeType, class EdgeType>
    static constexpr bool has_cost_v = estd::is_detected<has_cost_t, T, NodeType, EdgeType>::value;
    /**
     * @brief T has a member function double heuristic(const NodeType& from, const NodeType& to) noexcept
     */
    template<class T, class NodeType>
    using has_heuristic_t = decltype(std::declval<T>().heuristic(std::declval<const NodeType&>(), std::declval<const NodeType&>()));
    template<class T, class NodeType>
    static constexpr bool has_heuristic_v = estd::is_detected<has_heuristic_t, T, NodeType>::value;
    /**
     * @brief T is a Traversal Policy.
     * @details
     *  T has a member function bool canTraverse(const NodeType&, const EdgeType&, const NodeType&) noexcept;
     *  T has a member function double cost(const NodeType&, const EdgeType&, const NodeType&) noexcept;
     *  T has a member function double heuristic(const NodeType&, const NodeType&) noexcept;
     */
    template<class T, class NodeType, class EdgeType>
    struct is_a_traversal_policy{
        static constexpr bool value = has_can_traverse_v<T, NodeType, EdgeType> &&
                                        has_cost_v<T, NodeType, EdgeType> &&
                                        has_heuristic_v<T, NodeType>;
    };
    template<class T, class NodeType, class EdgeType>
    static constexpr bool is_a_traversal_policy_v = is_a_traversal_policy<T, NodeType, EdgeType>::value;
}

#endif // EVOAI_TYPE_UTILS_HPP
