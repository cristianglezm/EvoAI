#ifndef EVOAI_POPULATION_HPP
#define EVOAI_POPULATION_HPP

#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <random>
#include <string>

#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/Species.hpp>
#include <EvoAI/Genome.hpp>
#include <EvoAI/Export.hpp>
#include <EvoAI/SelectionAlgorithms.hpp>

#include <JsonBox.h>

namespace EvoAI{
    /**
     * @class Population<T>
     * @warning You can't use Population<T*>(JsonBox::Object) or Population<T*>(const std::string& filename) to load a Population.json
     *          also you can't use Population<T*>(std::size_t size, Args...args)
     * @brief Population<T> is an utility class that allows to evolve a population easily.
     * @details
     *   T needs to fulfill these conditions: <br />
     *      T has a member function JsonBox::Value toJson() const noexcept <br />
     *      T has a constructor T::T(JsonBox::Object) <br />
     *      T has a member function const double& getFitness() const noexcept <br />
     *      T has a member function void setFitness(const double&) noexcept <br />
     *      T has a static member function static double T::distance(const T&, const T&, double, double, double) noexcept <br />
     *      T has a static member function static T T::reproduce(const T&, const T&) noexcept <br />
     *   If Population<T*> it will act as an observer, what does this means: <br />
     *      Population<T*>::addMember will take a T* <br />
     *   If Population<T> it will act as an owner, what does this means: <br />
     *      Population<T>::addMember will take a T&& <br />
     * @author Cristian Glez <Cristian.glez.m@gmail.com>
     */
    template<typename T>
    class EvoAI_API Population{
            static_assert(is_populable_v<typename std::remove_pointer_t<T>>, "T needs to be Populable, more info at Population.hpp");
        public:
            using value_type = T;
            using reference = std::remove_reference_t<std::remove_pointer_t<T>>&;
            using rvalue_reference = std::remove_reference_t<std::remove_pointer_t<T>>&&;
            using const_reference = const std::remove_reference_t<std::remove_pointer_t<T>>&;
            using pointer = std::remove_pointer_t<T>*;
            using const_pointer = const std::remove_pointer_t<T>*;
            using species_map = std::map<std::size_t, std::unique_ptr<Species<value_type>>>;
            /**
             *  @brief result_t returned by reproduce and regrowPopulationFromElites when Population is an observer.
             *  returns a pair of vector<T*> and vector<T>, the first one is used to replace
             *  members that are not performing well, the second is the new members to be added.
             */
            using result_t = typename std::pair<std::vector<pointer>, std::vector<std::remove_pointer_t<std::remove_reference_t<T>>>>;
            /**
             *  @brief Utility to make a result_t, used by reproduce and regrowPopulationFromElites to manage ownership.
             *  @return result_t
             */
            static result_t make_result() noexcept{
                return std::make_pair(std::vector<pointer>(), std::vector<std::remove_pointer_t<std::remove_reference_t<T>>>());
            }
            using result_or_void_t = typename std::conditional_t<std::is_pointer_v<T>, result_t, void>;
            using species_ids_or_void_t = typename std::conditional_t<std::is_pointer_v<T>, std::vector<std::size_t>, void>;
        public:
            /**
             * @brief basic constructor
             * @return Population<T>
             */
            Population();
            /**
             * @brief creates a population of size with provided function, if using T* remember to avoid memory re-allocations.
             * @param fn std::function<T()>&& generator function
             * @param size std::size_t number of members
             * @return Population<T>
             */
            Population(std::function<T()>&& fn, std::size_t size);
            /**
             * @brief creates a population of specified size with the args provided.
             * @param size std::size_t number of members.
             * @tparam args Args...
             * @return Population<T>
             * @warning It cannot be used with T* use "Population(std::function<T()>&& fn, std::size_t size)" instead.
             */
            template<typename...Args>
            Population(std::size_t size, Args...args);
            /**
             * @brief Constructor for JsonBox::Object
             * @param o JsonBox::Object
             * @return Population<T>
             */
            Population(JsonBox::Object o);
            /**
             * @brief Constructor to load a Population from a json file.
             * @param filename loads a json file written with Population::writeTofile
             * @return Population<T>
             */
            Population(const std::string& filename);
            /**
             * @brief Adds a T, It will set T ID if owning it and it will check species, if it is not compatible with any other it will make a new species.
             * @param m T&&     if Population<T> otherwise T* if Population<T*>
             * @param c1 double coefficient for disjoints nodes and connections importance
             * @param c2 double coefficient for excess nodes and connections importance
             * @param c3 double coefficient for avg weight differences importance
             * @warning Invalidates iterators.
             */
            void addMember(std::conditional_t<std::is_pointer_v<value_type>, pointer, rvalue_reference> m, double c1 = 2.0, double c2 = 2.0, double c3 = 1.0) noexcept;
            /**
             * @brief removes a T and his species if its left empty.
             * @param m const T&
             * @warning Invalidates iterators.
             */
            void removeMember(reference m) noexcept;
            /**
             * @brief removes a list of T and his species if its left empty, use this if you need to remove many T* without invalidating them.
             *  
             * @param [in] membrs std::vector<T*> members to remove
             * @warning Invalidates iterators.
             */
            void removeMembers(std::vector<Population<T>::pointer>&& membrs) noexcept;
            /**
             * @brief adds a Species<T>, doesn't change its ID but it will overwrite the species if already added.
             * @param sp std::unique_ptr<Species<T>>&&
             */
            void addSpecies(std::unique_ptr<Species<value_type>>&& sp) noexcept;
            /**
             * @brief removes a Species<T>
             * @param sp const Species&
             * @warning Invalidates iterators.
             */
            void removeSpecies(const Species<value_type>& sp) noexcept;
            /**
             * @brief finds a member, returns nullptr if not found
             * @param id std::size_t member ID
             * @return T* or nullptr if not found
             */
            pointer findMember(std::size_t id) noexcept;
            /**
             * @brief gets the best member it has in the Population.
             * @return T* or nullptr if not found.
             */
            pointer getBestMember() noexcept;
            /**
             * @brief finds a Species, returns nullptr if not found.
             * @param id std::size_t Species ID
             * @return Species* or nullptr if not found.
             */
            Species<value_type>* findSpecies(std::size_t id) noexcept;
            /**
             *  @brief evaluate the members of the population.
             *  @param fn   std::function<void(T&)>&&
             */
            void eval(std::function<void(reference)>&& fn) noexcept;
            /**
             *  @brief returns the population cache
             *  @warning The pointers will get invalidated if added or removed a species or member to the population.
             *  @return std::vector<T*>&
             */
            std::vector<pointer>& getMembers() noexcept;
            /**
             *  @brief returns the species_map
             *  @return species_map&
             */
            species_map& getSpecies() noexcept;
            /**
             *  @brief clears all species and their members.
             */
            void clear() noexcept;
            /**
             *  @brief It removes species older than Population::maxAge or that are empty.
             *  @details it returns the ids of the species that had been removed when T* otherwise void.
             *  @code
             *      std::vector<T> storage(...);
             *      storage.reserve(maxPop);
             *      // createMembers can use a stack / queue / etc to check dead members to 
             *      // replace them or add new members. (possible implementation at PopulationTest.hpp)
             *      Population<T*> p(createMembers, 100);
             *      // more code
             *      auto ids = p.removeOldSpecies();
             *      // will check all members and those that their speciesID match will be
             *      // added to deadMembers. (possible implementation at PopulationTest.hpp)
             *      addToDeadMembers(ids);
             *  @endcode
             *  @return species_ids_or_void_t std::vector<std::size_t> when T* or void if T
             */
            species_ids_or_void_t removeOldSpecies() noexcept;
            /**
             *  @brief Increases the age by 1 or 2 if species is stagnant.
             *  @details if there are novel species it will make them not novel, if a species is
             *          stagnant it will increase its age by 2 otherwise by 1.
             */
            void increaseAge() noexcept;
            /**
             *  @brief Increases the age by 1 or 2 if species is stagnant and removes species that are over
             *          Population<T>::maxAge or empty.
             *  @details if there are novel species it will make them not novel, if a species is
             *          stagnant it will increase its age by 2 otherwise by 1, it will remove those
             *          species that are greater than maxAge or empty, it returns the ids of the species that had been removed when T* otherwise void.
             *  @return species_ids_or_void_t std::vector<std::size_t> when T* or void if T
             */
            species_ids_or_void_t increaseAgeAndRemoveOldSpecies() noexcept;
            /**
             *  @brief Add new members to the population.
             *  @param [in] args arguments for constructor of T
             *  @warning It cannot be used if using T* use "Population<T*>::regrowPopulation(Fn&& fn)".
             */
            template<typename...Args>
            void regrowPopulation(Args...args) noexcept;
            /**
             *  @brief Add new members to the population, remember to assign an id to T when using Population<T*>.
             *  @code
             *      std::vector<T> storage;
             *      storage.reserve(maxPop);
             *      Population<T*> pop(createMembers, maxPop); // createMembers possible implementation at PopulationTest.hpp
             *      // more code
             *      pop.regrowPopulation(createMembers);
             *  @endcode
             *  @param [in] Fn fn function returning a T or T*
             */
            template<typename Fn>
            void regrowPopulation(Fn&& fn) noexcept;
            /**
             *  @brief regrow the population from the best members, the returned pair.first is always empty, 
             *         in case we are using removeOldSpecies or increaseAgeAndRemoveOldSpecies we also need to 
             *         call regrowPopulation in case *removeOldSpecies kills the whole population.
             *  @code
             *      // code
             *      Population<T*> pop(createMembers, maxPop);
             *      auto sa = SelectionAlgorithms::Truncation<Genome*>{maxPop};
             *      // more code
             *      auto res = p.regrowPopulationFromElites(sa, false);
             *      replace(res.first, res.second); // replace possible implementation at PopulationTest.hpp
             *      // more code
             *  @endcode
             *  @param [in] SelectionAlgo     Selection algorithm to use.
             *  @param [in] interSpecies      if is permitted to reproduce between other species. 
             *  @return result_or_void_t      void if Population is the owner result_t otherwise.
             */
            template<typename SelectionAlgo>
            result_or_void_t regrowPopulationFromElites(SelectionAlgo&& sa, bool interSpecies) noexcept;
            /**
             *  @brief Replaces the members with less fitness of the Population with the children of the selected couples.
             *  @details In case you want your own selection algorithm you will need to implement a functor overloading operator()
             *              This one will handle interspecies reproduction by selecting couples from members
             *  
             *                 std::vector<Selected<T>> operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept;
             *  
             *              and this will handle species reproduction by selecting couples from species
             *  
             *                 std::vector<Selected<T>> operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept;
             *  
             *              they must return a std::vector<Selected<T>> where every Selected<T>::loser should be unique.
             *  @code
             *      // code
             *      Population<T*> pop(createMembers, maxPop);
             *      auto sa = SelectionAlgorithms::Truncation<Genome*>{maxPop};
             *      // more code
             *      auto res = p.reproduce(sa, false);
             *      replace(res.first, res.second); // replace possible implementation at PopulationTest.hpp
             *      // more code
             *  @endcode
             *  @param [in] sa           Selection algorithm to use
             *  @param [in] interSpecies if is permitted to reproduce between other species.
             *  @return result_or_void_t void if Population is the owner result_t otherwise.
             */
            template<typename SelectionAlgo>
            result_or_void_t reproduce(SelectionAlgo&& sa, bool interSpecies) noexcept;
            /**
             * @brief orders the members from the population by fitness
             */
            void orderMembersByFitness() noexcept;
            /**
             * @brief orders the members from the population by their ids
             */
            void orderMembersByID() noexcept;
            /**
             * @brief Orders all the members inside their own species by fitness.
             */
            void rankWithinSpecies() noexcept;
            /**
             * @brief returns the population current size.
             * @return std::size_t
             */
            std::size_t getPopulationSize() noexcept;
            /**
             *  @brief sets the max size of the population, use regrowPopulation to regrow to max population.
             *  
             *  @param [in] maxSize max size for population
             */
            void setPopulationMaxSize(std::size_t maxSize) noexcept;
            /**
             * @brief returns the population Max Size.
             * @return std::size_t
             */
            std::size_t getPopulationMaxSize() const noexcept;
            /**
             * @brief return number of species.
             * @return std::size_t
             */
            std::size_t getSpeciesSize() const noexcept;
            /**
             * @brief setter for maxAge
             * @param age std::size_t
             */
            void setMaxAge(std::size_t age) noexcept;
            /**
             * @brief returns maxAge
             * @param const std::size_t&
             */
            const std::size_t& getMaxAge() const noexcept;
            /**
             * @brief setter for the comparability Threshold
             * @param compThreshold const double&
             */
            void setCompatibilityThreshold(double compThreshold) noexcept;
            /**
             * @brief getter for the comparability Threshold
             * @return const double&
             */
            double getCompatibilityThreshold() const noexcept;
            /**
             * @brief computes the average fitness of the Population.
             * @return double
             */
            double computeAvgFitness() noexcept;
            /**
             * @brief returns a JsonBox::Value with the current info.
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief Writes the Population to a json file.
             * @param filename const std::string&
             */
            void writeToFile(const std::string& filename);
            ~Population() = default;
        private:
            std::size_t getNewSpeciesID() noexcept;
            std::size_t getNewMemberID() noexcept;
        private:
            species_map species;
            mutable std::vector<pointer> members;
            std::size_t PopulationSize;
            std::size_t maxAge;
            std::size_t speciesID;
            std::size_t memberID;
            double compatibilityThreshold;
            mutable bool membersCached;
    };
}

#include "Population.inl"

#endif // EVOAI_POPULATION_HPP