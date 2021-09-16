#ifndef EVOAI_SPECIES_HPP
#define EVOAI_SPECIES_HPP

#include <vector>
#include <memory>
#include <utility>
#include <random>
#include <string>

#include <EvoAI/Export.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>
#include <JsonBox.h>

namespace EvoAI{
    /**
     * @class Species
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     * @brief Class that represents a Species
     * @details
     *   T needs to fulfill these conditions: <br />
     *      T has a member function JsonBox::Value toJson() const noexcept <br />
     *      T has a constructor T::T(JsonBox::Object) <br />
     *      T has a member function const double& getFitness() const noexcept <br />
     *      T has a member function void setFitness(const double&) noexcept <br />
     *   If Species<T*> it will act as an observer what does this means: <br />
     *      Species<T*>::add(T* t) will accept a T* by value <br />
     *      Species<T*>::remove(T* t) will accept a T* by value <br />
     *      Species<T*>::has(T* t) will accept a T* by value <br />
     *   If Species<T> it will act as an owner what does this means: <br />
     *      Species<T>::add(T&& t) will take a T&& <br />
     *      Species<T>::remove(std::size_t id) will take an id by value <br />
     *      Species<T>::has(std::size_t id) will take an id by value <br />
     *  @warning You can't use Species<T*>(JsonBox::Object) or Species<T*>(const std::string& filename) to load a Species.json
     */
    template<typename T>
    class EvoAI_API Species{
            static_assert(is_speciable<typename std::remove_pointer_t<T>>::value, "T needs to be Speciable, more info at Species.hpp");
        public:
            using value_type = T;
            using reference = std::remove_reference_t<std::remove_pointer_t<T>>&;
            using rvalue_reference = std::remove_reference_t<std::remove_pointer_t<T>>&&;
            using const_reference = const std::remove_reference_t<std::remove_pointer_t<T>>&;
            using pointer = std::remove_pointer_t<T>*;
            using const_pointer = const std::remove_pointer_t<T>*;
        public:
            /**
             * @brief basic Constructor
             * @return Species<T>
             */
            Species() noexcept;
            /**
             * @brief copy Constructor
             * @return Species<T>
             */
            Species(const Species<T>& rhs) noexcept = default;
            /**
             * @brief move Constructor
             * @return Species<T>
             */
            Species(Species<T>&& rhs) noexcept = default;
            /**
             * @brief Creates a Species With the id and novel.
             * @param id std::size_T
             * @param novel bool if the species is a new one.
             * @return Species<T>
             */
            Species(const std::size_t& id, bool novel) noexcept;
            /**
             * @brief loads a Species from a JsonBox::Object, cannot be used with Species<T*>
             * @param o JsonBox::Object
             * @return Species<T>
             */
            Species(JsonBox::Object o) noexcept;
            /**
             * @brief It loads the species from a json file saved with Species::writeToFile, cannot be used with Species<T*>
             * @param filename const std::string&
             * @return Species<T>
             */
            Species(const std::string& filename);
            /**
             * @brief Adjust the fitness of the species dividing the fitness with the size of the species.
             */
            void adjustFitness() noexcept;
            /**
             * @brief computes the Average fitness
             */
            void computeAvgFitness() noexcept;
            /**
             * @brief computes the max fitness
             */
            void computeMaxFitness() noexcept;
            /**
             * @brief orders by fitness.
             */
            void rank() noexcept;
            /**
             * @brief returns the first member of this species
             * @return const T*
             */
            Species<T>::const_pointer getRepresentative() const noexcept;
            /**
             * @brief returns the best performing member of the species.
             * @return T*
             */
            Species<T>::pointer getChampion() noexcept;
            /**
             * @brief returns if is novel.
             * @return bool
             */
            bool isNovel() const noexcept;
            /**
             * @brief setter for novel
             * @param n bool
             */
            void setNovel(bool n) noexcept;
            /**
             * @brief returns if is killable
             * @return bool
             */
            bool isKillable() const noexcept;
            /**
             * @brief setter for killable
             * @param k bool killable
             */
            void setKillable(bool k) noexcept;
            /**
             * @brief sets the ID of the species.
             * @param speciesID const std::size_t&
             */
            void setID(const std::size_t& speciesID) noexcept;
            /**
             * @brief returns Species ID.
             * @return const std::size_t&
             */
            const std::size_t& getID() const noexcept;
            /**
             * @brief setter for the age of the species.
             * @param speciesAge const std::size_t&
             */
            void setAge(const std::size_t& speciesAge) noexcept;
            /**
             * @brief adds the amount to the current age of the species.
             * @param amount const std::size_t&
             */
            void addAge(const std::size_t& amount) noexcept;
            /**
             * @brief returns the current age of the species.
             */
            const std::size_t& getAge() const noexcept;
            /**
             * @brief setter for members.
             */
            void setMembers(std::vector<T>&& ms) noexcept;
            /**
             * @brief returns the members from this species.
             * @return std::vector<T>&
             */
            std::vector<T>& getMembers() noexcept;
            /**
             * @brief adds a member to the species and sets its SpeciesID to Species::getID().
             * @param m T&& if T is not a pointer T* otherwise.
             */
            void add(std::conditional_t<std::is_pointer_v<value_type>,pointer,rvalue_reference> m) noexcept;
            /**
             * @brief removes a member from the species.
             * @param id std::size_t if T is not a pointer T* otherwise.
             */
            void remove(std::conditional_t<std::is_pointer_v<value_type>, pointer, std::size_t> id) noexcept;
            /**
             * @brief checks if this T is in this species.
             * @param id std::size_t if T is not a pointer T* otherwise.
             * @return bool
             */
            bool has(std::conditional_t<std::is_pointer_v<value_type>, pointer, std::size_t> id) noexcept;
            /**
             * @brief orders the members from the species by ID.
             */
            void orderMembersByID() noexcept;
            /**
             * @brief Call Species<T>::computeAvgFitness first, returns the average fitness of the species.
             * @return double
             */
            double getAvgFitness() const noexcept;
            /**
             * @brief Call Species<T>::computeMaxFitness first, returns the max fitness that it has.
             * @return double
             */
            double getMaxFitness() const noexcept;
            /**
             * @brief returns the number of members in the species.
             * @return std::size_t
             */
            std::size_t getSize() const noexcept;
            /**
             *  @brief checks if is empty.
             *  
             *  @return bool
             */
            bool empty() const noexcept;
            /**
             * @brief checks if has stopped getting better
             * it needs to have Species<T>::computeAvgFitness run first.
             * @return bool
             */
            bool isStagnant() const noexcept;
            /**
             * @brief returns a JsonBox::Value with the current info.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief writes the Species to a file
             * @param filename
             */
            void writeToFile(const std::string& filename) const noexcept;
            ~Species() = default;
        public:
            Species<T>& operator=(const Species<T>& rhs) noexcept = default;
            Species<T>& operator=(Species<T>&& rhs) noexcept = default;
        private:
            std::vector<T> members;
            std::size_t id;
            std::size_t age;
            double avgFitness;
            double maxFitness;
            double oldAvgFitness;
            bool novel;
            bool killable;
    };
}

#include "Species.inl"

#endif // EVOAI_SPECIES_HPP