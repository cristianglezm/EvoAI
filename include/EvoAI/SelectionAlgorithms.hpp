#ifndef EVOAI_SELECTION_ALGORITHMS_HPP
#define EVOAI_SELECTION_ALGORITHMS_HPP

#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <random>
#include <string>
#include <type_traits>

#include <EvoAI/Species.hpp>
#include <EvoAI/Export.hpp>

namespace EvoAI{
    /**
     *  @brief Algorithms of Selection Strategies.
     *  @details In case you want your own selection algorithm you will need to implement a functor overloading operator()
     *           they must return a std::vector<Selected<T>> where every Selected<T>.loser should be unique.
     *           The algorithm can return less than the number of selected required, and should handle T and T*.
     *  @code
     *      template<typename T>
     *      struct EvoAI_API new_algorithm{
     *          //This one will handle interspecies reproduction by selecting couples from members
     *          std::vector<Selected<T>> operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept;
     *          //and this will handle species reproduction by selecting couples from species
     *          std::vector<Selected<T>> operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept;
     *      };
     *  @endcode
     */
    namespace SelectionAlgorithms{
        /**
         *  @brief Standard container for SelectionAlgorithms
         */
        template<typename T>
        struct Selected{
            using pointer = std::remove_pointer_t<T>*;
            pointer father;
            pointer mother;
            pointer loser;
            Selected(pointer father, pointer mother, pointer loser) noexcept
            : father(father)
            , mother(mother)
            , loser(loser){}
        };
        /**
         * @brief Truncation Selection algorithm
         * @details When using interspecies it will order members by fitness and then select couples from the fittest 
         *          members and a loser from the bottom of the list, when using species only it will select the fittest 
         *          members from the species by using a percentage calculated from the size of the species, the bigger 
         *          the species the more selected members from that species.
         */
        template<typename T>
        struct EvoAI_API Truncation{
            static_assert(get_fitness_v<std::remove_pointer_t<T>>, "T needs to have getFitness(), more info at SelectionAlgorithms.hpp");
            using pointer = std::remove_pointer_t<T>*;
            std::size_t maxPopulation;
            /**
             *  @brief constructor
             *  
             *  @param [in] maxPop maxPopulation 
             */
            Truncation(std::size_t maxPop) noexcept;
            /**
             *  @brief selects couples independent of their species.
             *  @param [in] std::vector<std::remove_pointer_t<T>*>& members
             *  @param [in] std::size_t numberToSelect
             *  @return std::vector<Selected<T>> selected 
             */
            std::vector<Selected<T>> operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept;
            /**
             *  @brief select couples of the same species.
             *  @param [in] std::map<std::size_t, std::unique_ptr<Species<T>>>& species
             *  @param [in] std::size_t numberToSelect
             *  @return std::vector<Selected<T>> selected
             */
            std::vector<Selected<T>> operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept;
        };
        /**
         *  @brief Tournament Selection algorithm
         *  @details It will randomly select members and compare their fitness, choosing a new contender for each round, the winner will be a parent,
         *          when using species only it will select more members of the biggest species.
         */
        template<typename T>
        struct EvoAI_API Tournament{
            static_assert(get_fitness_v<std::remove_pointer_t<T>>, "T needs to have getFitness(), more info at SelectionAlgorithms.hpp");
            using pointer = std::remove_pointer_t<T>*;
            std::size_t maxPop;
            std::size_t rounds;
            /**
             *  @brief constructor
             *  @param [in] maxPopulation maxPopulation used to calculate the number To select per species.
             *  @param [in] rounds rounds to select a champion and loser. 
             */
            Tournament(std::size_t maxPopulation, std::size_t rounds = 10) noexcept;
            /**
             *  @brief randomly selects some members and compare their fitness, 
             *  the champion is the one with the highest fitness.
             *  
             *  @tparam [in] members Members vector<T> for T and T*
             *  @return std::pair<pointer, pointer> champion and loser
             */
            template<typename Members>
            std::pair<pointer, pointer> fight(Members& members) noexcept;
            /**
             *  @brief selects couples independent of their species.
             *  @param [in] std::vector<std::remove_pointer_t<T>*>& members
             *  @param [in] std::size_t numberToSelect
             *  @return std::vector<Selected<T>> selected 
             */
            std::vector<Selected<T>> operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept;
            /**
             *  @brief select couples of the same species.
             *  @param [in] std::map<std::size_t, std::unique_ptr<Species<T>>>& species
             *  @param [in] std::size_t numberToSelect
             *  @return std::vector<Selected<T>> selected
             */
            std::vector<Selected<T>> operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept;
        };
        /**
         *  @brief Fitness Proportionate Selection aka roulette wheel selection algorithm
         *  @details calculates the proportion using fitness and randomly selects a couple and a loser.
         */
        template<typename T>
        struct EvoAI_API FPS{
            static_assert(get_fitness_v<std::remove_pointer_t<T>>, "T needs to have getFitness(), more info at SelectionAlgorithms.hpp");
            using pointer = std::remove_pointer_t<T>*;
            std::size_t maxPop;
            /**
             *  @brief constructor
             *  @param [in] maxPopulation maxPopulation used to calculate the number To select per species.
             */
            explicit FPS(std::size_t maxPopulation) noexcept;
            /**
             *  @brief randomly select a number and select a member that the sum of its fitness is over that number.
             *  @tparam [in] members Members vector<T> for T and T*
             *  @return T* it can be a nullptr
             */
            template<typename Members>
            pointer FPSelection(Members& members, double totalFitness) noexcept;
            /**
             *  @brief selects couples independent of their species.
             *  @param [in] std::vector<std::remove_pointer_t<T>*>& members
             *  @param [in] std::size_t numberToSelect
             *  @return std::vector<Selected<T>> selected 
             */
            std::vector<Selected<T>> operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept;
            /**
             *  @brief select couples of the same species.
             *  @param [in] std::map<std::size_t, std::unique_ptr<Species<T>>>& species
             *  @param [in] std::size_t numberToSelect
             *  @return std::vector<Selected<T>> selected
             */
            std::vector<Selected<T>> operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept;
        };
    }
}
#include "SelectionAlgorithms.inl"

#endif // EVOAI_SELECTION_ALGORITHMS_HPP