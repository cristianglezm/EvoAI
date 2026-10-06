#ifndef EVOAI_SOMTRAINING_HPP
#define EVOAI_SOMTRAINING_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

namespace EvoAI{
    /**
     * @brief what one epoch of SelfOrganizingMap::train() did.
     */
    struct EpochInfo final{
        /// index of the epoch in the whole schedule: TrainingOptions::epochOffset plus the epochs run before it in this call
        std::size_t epoch = 0u;
        /// learning rate used in this epoch
        double learningRate = 0.0;
        /// neighbourhood radius used in this epoch
        double radius = 0.0;
        /**
         * @brief mean distance from each sample to its best matching unit while training.
         * @details
         *  The weights keep moving during the epoch, so this is a little different from the
         *  quantization error SelfOrganizingMap::evaluate() measures afterwards. It costs nothing
         *  extra: the search for the winner computes it anyway.
         */
        double meanBmuDistance = 0.0;
    };
    /**
     * @brief optional settings of SelfOrganizingMap::train().
     */
    struct TrainingOptions final{
        /**
         * @brief epochs of the schedule already done, to continue it in a later call.
         * @details
         *  The schedulers are replayed from epoch 0 up to this offset before the first epoch of
         *  the call, so training 10 epochs in one call or 5 and then 5 with epochOffset 5 follows the
         *  same learning rate and radius for any scheduler.
         */
        std::size_t epochOffset = 0u;
        /**
         * @brief seed of the order the samples are visited in.
         * @details
         *  Empty: the shared randomGen() decides. With a seed the order of every epoch depends only on the
         *  seed and the epoch index, so training in pieces gives the same weights as one call
         *  (same standard library, same platform).
         */
        std::optional<std::uint64_t> seed;
        /**
         * @brief called after every epoch, return false to stop training after that epoch.
         */
        std::function<bool(const EpochInfo&)> onEpoch;
    };
    /**
     * @brief what SelfOrganizingMap::train() did.
     */
    struct TrainingReport final{
        /// one entry per epoch that ran
        std::vector<EpochInfo> epochs;
        /// indices in the samples given of those that failed SelfOrganizingMap::validateSample() and were ignored
        std::vector<std::size_t> skipped;
        /// samples won by each node in the last epoch (flat index y * width + x), all 0 if no epoch ran
        std::vector<std::size_t> nodeCounts;
    };
}

#endif // EVOAI_SOMTRAINING_HPP
