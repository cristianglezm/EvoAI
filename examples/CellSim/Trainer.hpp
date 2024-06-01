#ifndef EVOAI_TRAINER_HPP
#define EVOAI_TRAINER_HPP

#include <EvoAI/Genome.hpp>
#include <EvoAI/NeuralNetwork.hpp>
#include <EvoAI/DataLoader.hpp>
#include <EvoAI/DataSet.hpp>

namespace EvoAI{
    /**
     * @brief A class to train Cells genomes.
     */
    class Trainer final{
        public:
            /**
             * @brief A Trainer to train Cells.
             * @param samples std::size_t
             * @param batchSize std::size_t
             * @param epoch std::size_t
             * @param lr double
             */
            Trainer(std::size_t samples, std::size_t batchSize, std::size_t epoch, double lr) noexcept;
            /**
             * @brief trains a genome and returns its trained Genome back
             * @param g Genome&&
             * @return Genome
             */
            Genome train(Genome&& g) noexcept;
            /**
             * @brief add sample to TrainingSet
             * @param sample std::pair<std::vector<double>, std::vector<double>>&&
             */
            void addToTrainingSet(std::pair<std::vector<double>, std::vector<double>>&& sample) noexcept;
            /**
             * @brief add sample to TestSet
             * @param sample 
             */
            void addToTestSet(std::pair<std::vector<double>, std::vector<double>>&& sample) noexcept;
            /**
             * @brief generates a sample for the Trainer.
             * @return std::pair<std::vector<double>, std::vector<double>>
             */
            static std::pair<std::vector<double>, std::vector<double>> generateSample() noexcept;
        private:
            /**
             * @brief helper to construct Trainer
             * @param samples std::size_t 
             * @param batchSize std::size_t
             * @return DataLoader<Dataset>
             */
            DataLoader<Dataset> init(std::size_t samples, std::size_t batchSize) noexcept;
        private:
            DataLoader<Dataset> trainingSet;
            DataLoader<Dataset> testSet;
            std::size_t mBatchSize;
            std::size_t mSamples;
            std::size_t mEpoch;
            double mLr;
    };
}

#endif // EVOAI_TRAINER_HPP