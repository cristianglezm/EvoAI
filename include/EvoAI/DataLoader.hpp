#ifndef EVOAI_DATALOADER_HPP
#define EVOAI_DATALOADER_HPP

#include <vector>
#include <random>
#include <cassert>

#include <EvoAI/Config.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>

namespace EvoAI{
    /**
     * @brief Dataloader to process Dataset
     * @details
     * Dataset needs to fulfill there requirements: <br />
     * * const std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept;<br />
     * * std::size_t size() const noexcept;<br />
     * * std::size_t getBatchSize() const noexcept;<br />
     * * void shuffle() noexcept;<br />
     * @code
     *     EvoAI::DataLoader trainingDataset(EvoAI::Dataset(std::move(inputs), std::move(outputs), batchSize));
     * @endcode
     * @tparam Dataset 
     */
    template<class Dataset>
    class EvoAI_API DataLoader final{
        public:
            using type = Dataset;
            static_assert(meta::is_a_dataset_v<Dataset>, "Dataset needs to be a dataset, more info at DataLoader.hpp");
        public:
            /**
             * @brief constructor
             * @param ds Dataset
             * @param randomize bool
             */
            DataLoader(Dataset&& ds, bool randomize = true);
            /**
             * @brief next sample
             * @return std::pair<std::vector<double>&, std::vector<double>&>
             */
            std::pair<std::vector<double>&, std::vector<double>&> operator()() noexcept;
            /**
             * @brief size of (samples + batchSize - 1) / batchSize
             * @return std::size_t
             */
            std::size_t size() noexcept;
            /**
             * @brief gets batchSize
             * @return std::size_t
             */
            std::size_t getBatchSize() noexcept;
            /**
             * @brief shuffle the samples
             */
            void shuffle() noexcept;
            /**
             * @brief gets direct access to Dataset
             * @return Dataset&
             */
            Dataset& getDataset() noexcept;
        private:
            bool m_randomize;
            Dataset m_ds;
    };
}
#include "DataLoader.inl"
#endif  // EVOAI_DATALOADER_HPP