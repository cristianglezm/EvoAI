#ifndef EVOAI_EVOVECTOR_HPP
#define EVOAI_EVOVECTOR_HPP

#include <vector>
#include <limits>
#include <algorithm>

#include <EvoAI/Export.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>
#include <JsonBox.h>

namespace EvoAI{
	/**
	 * @brief Simple class representing a Evolvable Vector.
	 * @tparam T
	 */
	template<typename T>
	class EvoAI_API EvoVector final{
		public:
			using value_type = typename std::vector<T>::value_type;
			using reference = typename std::vector<T>::reference;
			using const_reference = typename std::vector<T>::const_reference;
			using iterator = typename std::vector<T>::iterator;
			using const_iterator = typename std::vector<T>::const_iterator;
			using DNA = typename std::vector<T>;
		public:
			/**
			 * @brief constructor
			 * @param size std::size_t
			 */
			explicit EvoVector(std::size_t size) noexcept;
			/**
			 * @brief constructor with generator fn
			 * @param size std::size_t
			 * @param fn Generator&&
			 */
			template<typename Generator>
			explicit EvoVector(std::size_t size, Generator&& fn) noexcept;
			/**
			 * @brief constructor for EvoVector children
			 * @param dna DNA&&
			 */
			explicit EvoVector(DNA&& dna) noexcept;
			/**
			 * @brief constructor JsonBox::Object
			 * @param o JsonBox::Object
			 */
			EvoVector(JsonBox::Object o) noexcept;
			/**
			 * @brief converts to json
			 * @return JsonBox::Value
			 */
			JsonBox::Value toJson() const noexcept;
			/**
			 * @brief pointer to internal vector.data()
			 * @return T*
			 */
			value_type* data() noexcept;
			/**
			 * @brief sets the EvoVector id
			 * @param ID std::size_t
			 */
			inline void setID(std::size_t ID) noexcept;
			/**
			 * @brief gets the EvoVector id
			 * @return std::size_t
			 */
			inline std::size_t getID() const noexcept;
			/**
			 * @brief setter for species ID
			 * @param spcID species id
			 */
			inline void setSpeciesID(std::size_t spcID) noexcept;
			/**
			 * @brief sets the fitness
			 * @param fit double
			 */
			inline void setFitness(double fit) noexcept;
			/**
			 * @brief adds the amount to the current fitness.
			 * @param amount double
			 */
			inline void addFitness(double amount) noexcept;
			/**
			 * @brief gets the fitness
			 * @return double
			 */
			inline double getFitness() const noexcept;
			/**
			 * @brief returns the species id.
			 * @return std::size_t
			 */
			inline std::size_t getSpeciesID() const noexcept;
			/**
			 * @brief returns size
			 * @return std::size_t
			 */
			inline std::size_t size() const noexcept;
			/**
			 * @brief mutates the EvoVector
			 * @param rate float
			 */
			void mutate(float rate = 0.5) noexcept;
		public:
			inline reference operator[](int index) noexcept;
			inline const_reference operator[](int index) const noexcept;
			inline iterator begin() noexcept;
			inline iterator end() noexcept;
			inline const_iterator begin() const noexcept;
			inline const_iterator end() const noexcept;
		public:
			/**
			 * @brief reproduce
			 * @param es1 const EvoVector&
			 * @param es2 const EvoVector&
			 * @return EvoVector 
			 */
			static EvoVector reproduce(const EvoVector& es1, const EvoVector& es2) noexcept;
			/**
			 * @brief Calculates the distance between two EvoVectors. 1000 if size dont match up.
			 * @param es1 EvoVector
			 * @param es2 EvoVector
			 * @param c1 coefficient Gives importance to [not used]
			 * @param c2 coefficient Gives importance to [not used]
			 * @param c3 coefficient Gives importance to [not used]
			 * @return double
			 */
			static double distance(const EvoVector& es1, const EvoVector& es2, 
					[[maybe_unused]] double c1 = 2.0, 
					[[maybe_unused]] double c2 = 2.0, 
					[[maybe_unused]] double c3 = 1.0) noexcept;
		private:
			DNA m_dna;
			std::size_t m_Id;
			std::size_t m_speciesId;
			double m_fitness;
	};
	using EvoString = EvoVector<char>;
}
#include "EvoVector.inl"
#endif // EVOAI_EVOVECTOR_HPP