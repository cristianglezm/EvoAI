#ifndef EVOAI_DATASET_HPP
#define EVOAI_DATASET_HPP

#include <EvoAI/Config.hpp>

#include <vector>

namespace EvoAI{
    /**
     * @brief Dataset
     * @code
     *     EvoAI::Dataset dataset(std::move(inputs), std::move(outputs), batchSize);
     * @endcode
     */
    class EvoAI_API Dataset final{
        public:
            /**
             * @brief expected input and target type
             */
            using TrainingFormat = std::vector<std::pair<std::vector<double>, std::vector<double>>>;
        public:
            /**
             * @brief constructor for empty dataset.
             * @warning You will need to add samples with Dataset::add before using it.
             * @param batchSize 
             */
            Dataset(std::size_t batchSize) noexcept;
            /**
             * @brief constructor
             * @param data Dataset::TrainingFormat
             * @param batchSize std::size_t
             */
            Dataset(TrainingFormat&& data, std::size_t batchSize) noexcept;
            /**
             * @brief constructor
             * @param inputs std::vector<std::vector<double>>&&
             * @param targets std::vector<std::vector<double>>&&
             * @param batchSize std::size_t
             */
            Dataset(std::vector<std::vector<double>>&& inputs, std::vector<std::vector<double>>&& targets, std::size_t batchSize) noexcept;
            /**
             * @brief next sample
             * @return const std::pair<std::vector<double>&, std::vector<double>&>
             */
            const std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept;
            /**
             * @brief add sample to dataset.
             * @param sample std::pair<std::vector<double>, std::vector<double>>&&
             */
            void add(std::pair<std::vector<double>, std::vector<double>>&& sample) noexcept;
            /**
             * @brief size of (samples + batchSize - 1) / batchSize
             * @return std::size_t
             */
            std::size_t size() const noexcept;
            /**
             * @brief get BatchSize
             * @return std::size_t
             */
            std::size_t getBatchSize() const noexcept;
            /**
             * @brief method to shuffle the samples after the epoch
             */
            void shuffle() noexcept;
        private:
            std::size_t m_batchSize;
            mutable std::size_t m_index;
            TrainingFormat m_data;
    };
}

#endif // EVOAI_DATASET_HPP