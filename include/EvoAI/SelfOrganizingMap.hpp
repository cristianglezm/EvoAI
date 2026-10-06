#ifndef EVOAI_SELF_ORGANIZING_MAP_HPP
#define EVOAI_SELF_ORGANIZING_MAP_HPP

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <limits>
#include <random>
#include <stdexcept>
#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

#include <JsonBox.h>

#include <EvoAI/Schedulers.hpp>
#include <EvoAI/SOMEvaluation.hpp>
#include <EvoAI/SOMTraining.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>
#include <EvoAI/Utils/RandomUtils.hpp>

namespace EvoAI{
    /**
     * @brief Euclidean distance between two feature vectors.
     * @details
     *  Default DistanceFn for SelfOrganizingMap. Fulfills
     *  meta::distance_metric_v<EuclideanDistance>.
     */
    struct EuclideanDistance final{
        /**
         * @brief computes the euclidean distance between a and b.
         * @param a const double* first of n values
         * @param b const double* first of n values
         * @param n std::size_t how many values each one has
         * @return double
         */
        double operator()(const double* a, const double* b, std::size_t n) const noexcept;
    };

    /**
     * @brief winning node of a sample: its grid coordinate and how far the sample is from its prototype.
     * @details
     *  distance is what DistanceFn returned for the winner, so a caller that needs the
     *  quantization error of a sample does not have to fetch the prototype and measure again.
     */
    struct BestMatchingUnit final{
        std::size_t x;
        std::size_t y;
        double distance;
    };
    /**
     * @brief which grid nodes count as the neighbours of a node.
     */
    enum class Neighbourhood{
        Four, ///< left, right, up and down
        Eight ///< the four above plus the diagonals
    };
    /**
     * @class SelfOrganizingMap
     * @brief Generic Kohonen Self-Organizing Map over std::vector<double> feature vectors.
     * @tparam DistanceFn distance metric between two feature vectors, defaults to EuclideanDistance.
     * @details
     *
     *  DistanceFn needs to fulfill meta::distance_metric_v<DistanceFn>: <br />
     *      DistanceFn has a double operator()(const double* a, const double* b, std::size_t n) const noexcept <br />
     *      comparing the n values at a with the n values at b, without keeping any state <br />
     *
     *  Thread safety: classify(), evaluate(), uMatrix(), getPrototype() and toJson() only read the map,
     *  so any number of threads can call them at once as long as none of them is training or
     *  changing the map at the same time.
     */
    template<class DistanceFn = EuclideanDistance>
    class SelfOrganizingMap final{
        public:
            static_assert(meta::distance_metric_v<DistanceFn>, "DistanceFn requires to fulfill meta::distance_metric_v, more info at Utils/TypeUtils.hpp");
            /**
             * @brief constructor
             * @param width std::size_t grid width, must be > 0
             * @param height std::size_t grid height, must be > 0
             * @param inputDim std::size_t dimension every feature vector given to
             *        train()/classify() must have, must be > 0
             * @param toroidal bool whether the grid wraps around at the edges (default true)
             * @param distance DistanceFn distance metric to use (default EuclideanDistance{})
             * @throws std::invalid_argument if width, height or inputDim is 0 -
             *         this is a structural/construction-time error, not a runtime-data-quality one.
             */
            SelfOrganizingMap(std::size_t width, std::size_t height, std::size_t inputDim,
                               bool toroidal = true, DistanceFn distance = DistanceFn{});
            /**
             * @brief constructor for a JsonBox::Object saved with toJson()
             * @param o JsonBox::Object
             * @throws std::invalid_argument if width, height or inputDim is 0, if
             *         width*height*inputDim overflows std::size_t, or if the saved weights
             *         array's size doesn't match width*height*inputDim.
             */
            SelfOrganizingMap(JsonBox::Object o);
            /**
             * @brief checks whether sample is usable by train()/classify(): its
             *        size matches getInputDim() and every element is finite
             *        (not NaN/Inf).
             * @param sample const std::vector<double>&
             * @return bool
             */
            bool validateSample(const std::vector<double>& sample) const noexcept;
            /**
             * @brief trains the map with a batch of samples over a number of epochs.
             * @details
             *  Trains on batches by skipping invalid samples and reshuffling sample order each epoch.
             *  Learning-rate and radius schedules advance before each epoch, starting at epoch 0.
             *  Use TrainingOptions::epochOffset with the same schedulers and base values to continue
             *  training across multiple calls without restarting the schedule. Schedulers are passed
             *  by value, leaving the originals unchanged.
             *
             *  If the neighborhood radius reaches zero or below, only the BMU is updated, with full
             *  influence. Other nodes are skipped. Nodes with influence below 1e-12 are also skipped.
             *
             *  Weights with magnitude below 1e-100 are set to zero to avoid the performance cost of
             *  denormal floating-point values.
             *
             *  StepLR and LinearLR apply changes at epoch multiples of their step, including epoch 0.
             *  LinearLR uses an absolute decrement in the units of the scheduled value. MultiStepLR
             *  should be passed in its original state when continuing with epochOffset, so milestones
             *  are replayed correctly.
             *
             * @tparam LRAlgo Scheduler algorithm for the learning rate.
             * @tparam RadiusAlgo Scheduler algorithm for the neighborhood radius.
             * @param samples const std::vector<std::vector<double>>&
             * @param epochs std::size_t
             * @param baseLR double
             * @param baseRadius double
             * @param lrSchedule Scheduler<LRAlgo>
             * @param radiusSchedule Scheduler<RadiusAlgo>
             * @param options const TrainingOptions& offset into the schedule, seed and per-epoch callback
             * @return TrainingReport what every epoch did, the skipped samples and the last epoch's wins per node
             */
            template<class LRAlgo, class RadiusAlgo>
            TrainingReport train(const std::vector<std::vector<double>>& samples, std::size_t epochs,
                                 double baseLR, double baseRadius,
                                 Scheduler<LRAlgo> lrSchedule, Scheduler<RadiusAlgo> radiusSchedule,
                                 const TrainingOptions& options = {});
            /**
             * @brief finds the Best-Matching-Unit for a sample.
             * @details Samples that land on the same grid coordinate belong to the same
             *          cluster. When several nodes are equally close the first one in
             *          row-major order (y, then x) wins.
             * @param sample const std::vector<double>&
             * @return std::optional<BestMatchingUnit> empty if sample fails validateSample().
             */
            std::optional<BestMatchingUnit> classify(const std::vector<double>& sample) const noexcept;
            /**
             * @brief measures how well the map fits samples, in one pass over them.
             * @details
             *  Samples that fail validateSample() are counted in samplesSkipped and
             *  otherwise ignored. Nothing is trained or changed.
             * @param samples const std::vector<std::vector<double>>&
             * @param topographicError bool also compute the topographic error. It needs the
             *        second closest node of every sample, a little more work per node, so it is
             *        off unless asked for.
             * @param adjacency Neighbourhood which nodes count as neighbours for the topographic
             *        error, wrapping around the edges when the map is toroidal. A map with a single
             *        node has no second node, its samples count as well placed.
             * @return SOMEvaluation
             */
            SOMEvaluation evaluate(const std::vector<std::vector<double>>& samples, bool topographicError = false,
                                   Neighbourhood adjacency = Neighbourhood::Eight) const;
            /**
             * @brief the unified distance matrix (U-matrix): for every node, the mean distance
             *        from its prototype to the prototypes of its neighbours.
             * @details
             *  High values mark borders between clusters, low values the inside of one. Each
             *  distance is DistanceFn(prototype of the node, prototype of the neighbour).
             *  A neighbour that wraps around onto the node itself or onto another neighbour
             *  (maps only one or two nodes wide) is counted once. A node without
             *  neighbours (a map with a single node) gets 0.
             * @param neighbours Neighbourhood which nodes count as neighbours, wrapping around the
             *        edges when the map is toroidal.
             * @return std::vector<double> width * height values, flat index y * width + x
             */
            std::vector<double> uMatrix(Neighbourhood neighbours = Neighbourhood::Eight) const;
            /**
             * @brief copies out node (x,y)'s prototype vector.
             *
             * @param x std::size_t
             * @param y std::size_t
             * @return std::vector<double>
             * @throws std::out_of_range if x >= getWidth() or y >= getHeight().
             */
            std::vector<double> getPrototype(std::size_t x, std::size_t y) const;
            /**
             * @brief getter for grid width
             * @return std::size_t
             */
            std::size_t getWidth() const noexcept;
            /**
             * @brief getter for grid height
             * @return std::size_t
             */
            std::size_t getHeight() const noexcept;
            /**
             * @brief getter for the dimension every feature vector must have
             * @return std::size_t
             */
            std::size_t getInputDim() const noexcept;
            /**
             * @brief converts to a JsonBox::Value
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const;
            /**
             * @brief rounds the weights toJson() writes to this many decimals, to make the JSON shorter.
             * @details
             *  Empty (the default) writes every weight exactly. With a number the weights are rounded
             *  to that many decimals when written: a weight within half a unit of the last decimal of 0 or 1
             *  is written as exactly 0 or 1, and the others with just the digits they need (the writer
             *  drops the noise of the binary representation). The map itself keeps its exact weights, only
             *  the JSON changes, and a map loaded from it holds the rounded ones and can be trained as usual.
             *  The rounding is absolute: 4 decimals keep weights of the order of 1 to within 0.00005 but
             *  would turn weights of the order of 0.00001 into 0. Choose it for the scale of your data.
             *  It is not saved in the JSON, so a loaded map writes exact weights again until it is set.
             * @param decimals std::optional<int> from 0 to 17
             * @throws std::invalid_argument if decimals is outside 0 to 17.
             */
            void setWeightDecimals(std::optional<int> decimals);
            /**
             * @brief the number of decimals toJson() rounds the weights to, empty if it writes them exactly.
             */
            std::optional<int> getWeightDecimals() const noexcept;
            ~SelfOrganizingMap() = default;
        private:
            /**
             * @brief flat index of dimension d of node (x,y)'s prototype vector
             *        inside m_weights. Row-major: node (x,y)'s inputDim-length
             *        vector starts at idx(x,y,0) and runs for inputDim doubles.
             */
            std::size_t idx(std::size_t x, std::size_t y, std::size_t d) const noexcept;
            /**
             * @brief grid-space distance between two nodes (not feature-space -
             *        used for the neighborhood Gaussian), accounting for
             *        toroidal wraparound if enabled.
             */
            double gridDistance(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const noexcept;
            /**
             * @brief Best-Matching-Unit search over the whole grid.
             *
             * @param sample const std::vector<double>& sample to find bmu from.
             */
            BestMatchingUnit findBMU(const std::vector<double>& sample) const noexcept;
            /**
             * @brief the two closest nodes to a sample, the best first. The second is the best
             *        again when the map has a single node.
             */
            std::pair<BestMatchingUnit, BestMatchingUnit> findTwoBMUs(const std::vector<double>& sample) const noexcept;
            /**
             * @brief whether two different nodes are neighbours, accounting for toroidal wraparound.
             */
            bool adjacent(const BestMatchingUnit& a, const BestMatchingUnit& b, Neighbourhood n) const noexcept;
        private:
            std::size_t m_width;
            std::size_t m_height;
            std::size_t m_inputDim;
            bool m_toroidal;
            std::optional<int> m_weightDecimals{};
            DistanceFn m_distance;
            std::vector<double> m_weights;
    };
    // EuclideanDistance impl
    inline double EuclideanDistance::operator()(const double* a, const double* b, std::size_t n) const noexcept{
        double sum = 0.0;
        for(std::size_t i=0;i<n;++i){
            auto diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
    namespace detail{
        // A wrapped width*height*inputDim would pass the saved-weights size check with an
        // inconsistent grid, and classify() would then index past the weights.
        inline std::size_t somWeightCount(std::size_t width, std::size_t height, std::size_t inputDim){
            if(width == 0 || height == 0 || inputDim == 0){
                throw std::invalid_argument("EvoAI::SelfOrganizingMap - width, height and inputDim must all be > 0");
            }
            constexpr auto max = std::numeric_limits<std::size_t>::max();
            if(height > max / width || width * height > max / inputDim){
                throw std::invalid_argument("EvoAI::SelfOrganizingMap - width*height*inputDim overflows");
            }
            return width * height * inputDim;
        }
    }
    // SelfOrganizingMap impl
    template<class DistanceFn>
    SelfOrganizingMap<DistanceFn>::SelfOrganizingMap(std::size_t width, std::size_t height, std::size_t inputDim,
                                                       bool toroidal, DistanceFn distance)
    : m_width(width)
    , m_height(height)
    , m_inputDim(inputDim)
    , m_toroidal(toroidal)
    , m_distance(distance)
    , m_weights(detail::somWeightCount(width, height, inputDim)){
        for(auto& w : m_weights){
            w = randomGen().random(0.0, 1.0);
        }
    }
    template<class DistanceFn>
    SelfOrganizingMap<DistanceFn>::SelfOrganizingMap(JsonBox::Object o)
    : m_width(safeParseUInt<std::size_t>(o["width"].getString()))
    , m_height(safeParseUInt<std::size_t>(o["height"].getString()))
    , m_inputDim(safeParseUInt<std::size_t>(o["inputDim"].getString()))
    , m_toroidal(o["toroidal"].getBoolean())
    , m_distance(DistanceFn{})
    , m_weights(){
        // everything is checked against the weights actually present before anything is sized from
        // the dimensions in the JSON, which can claim any number
        auto weightsArr = o["weights"].getArray();
        if(weightsArr.size() != detail::somWeightCount(m_width, m_height, m_inputDim)){
            throw std::invalid_argument("EvoAI::SelfOrganizingMap - saved weights size doesn't match width*height*inputDim");
        }
        m_weights.reserve(weightsArr.size());
        for(auto& v : weightsArr){
            m_weights.push_back(v.getDouble());
        }
    }
    template<class DistanceFn>
    bool SelfOrganizingMap<DistanceFn>::validateSample(const std::vector<double>& sample) const noexcept{
        if(sample.size() != m_inputDim){
            return false;
        }
        return std::all_of(sample.begin(), sample.end(), [](double v){ return std::isfinite(v); });
    }
    template<class DistanceFn>
    template<class LRAlgo, class RadiusAlgo>
    TrainingReport SelfOrganizingMap<DistanceFn>::train(const std::vector<std::vector<double>>& samples, std::size_t epochs,
                                                       double baseLR, double baseRadius,
                                                       Scheduler<LRAlgo> lrSchedule, Scheduler<RadiusAlgo> radiusSchedule,
                                                       const TrainingOptions& options){
        constexpr double minInfluence = 1e-12;
        constexpr double tinyWeight = 1e-100;
        TrainingReport report;
        report.nodeCounts.assign(m_width * m_height, 0u);
        std::vector<std::size_t> validIndices;
        validIndices.reserve(samples.size());
        for(std::size_t i=0;i<samples.size();++i){
            if(validateSample(samples[i])){
                validIndices.push_back(i);
            }else{
                report.skipped.push_back(i);
            }
        }
        if(validIndices.empty()){
            return report;
        }
        double currentLR = baseLR;
        double currentRadius = baseRadius;
        for(std::size_t epoch=0;epoch<options.epochOffset;++epoch){
            currentLR = lrSchedule.step(currentLR, epoch);
            currentRadius = radiusSchedule.step(currentRadius, epoch);
        }
        // Fisher-Yates shuffle
        auto shuffle = [](std::vector<std::size_t>& order, auto& engine){
            for(std::size_t i=order.size(); i>1; --i){
                std::uniform_int_distribution<std::size_t> ud(0u, i-1);
                std::swap(order[i-1], order[ud(engine)]);
            }
        };
        auto order = validIndices;
        report.epochs.reserve(epochs);
        for(std::size_t done=0;done<epochs;++done){
            const auto epoch = options.epochOffset + done;
            currentLR = lrSchedule.step(currentLR, epoch);
            currentRadius = radiusSchedule.step(currentRadius, epoch);
            order = validIndices;
            if(options.seed){
                const auto seed = *options.seed;
                std::seed_seq sequence{static_cast<std::uint32_t>(seed), static_cast<std::uint32_t>(seed >> 32u),
                                       static_cast<std::uint32_t>(epoch), static_cast<std::uint32_t>(static_cast<std::uint64_t>(epoch) >> 32u)};
                std::mt19937_64 engine(sequence);
                shuffle(order, engine);
            }else{
                shuffle(order, randomGen().getEngine());
            }
            std::fill(report.nodeCounts.begin(), report.nodeCounts.end(), 0u);
            double distanceSum = 0.0;
            for(auto sampleIdx : order){
                const auto& sample = samples[sampleIdx];
                const auto bmu = findBMU(sample);
                distanceSum += bmu.distance;
                ++report.nodeCounts[bmu.y * m_width + bmu.x];
                for(std::size_t y=0;y<m_height;++y){
                    for(std::size_t x=0;x<m_width;++x){
                        auto gDist = gridDistance(x, y, bmu.x, bmu.y);
                        double influence;
                        if(currentRadius > 0.0){
                            influence = std::exp(-(gDist * gDist) / (2.0 * currentRadius * currentRadius));
                        }else{
                            influence = (gDist == 0.0) ? 1.0 : 0.0;
                        }
                        if(influence < minInfluence){
                            continue;
                        }
                        for(std::size_t d=0;d<m_inputDim;++d){
                            auto& weight = m_weights[idx(x, y, d)];
                            weight += currentLR * influence * (sample[d] - weight);
                            if(std::abs(weight) < tinyWeight){
                                weight = 0.0;
                            }
                        }
                    }
                }
            }
            EpochInfo info;
            info.epoch = epoch;
            info.learningRate = currentLR;
            info.radius = currentRadius;
            info.meanBmuDistance = distanceSum / static_cast<double>(order.size());
            report.epochs.push_back(info);
            if(options.onEpoch && !options.onEpoch(info)){
                break;
            }
        }
        return report;
    }

    template<class DistanceFn>
    std::optional<BestMatchingUnit> SelfOrganizingMap<DistanceFn>::classify(const std::vector<double>& sample) const noexcept{
        if(!validateSample(sample)){
            return std::nullopt;
        }
        return findBMU(sample);
    }
    template<class DistanceFn>
    std::vector<double> SelfOrganizingMap<DistanceFn>::getPrototype(std::size_t x, std::size_t y) const{
        if(x >= m_width || y >= m_height){
            throw std::out_of_range("EvoAI::SelfOrganizingMap::getPrototype - (x,y) out of range");
        }
        auto base = idx(x, y, 0);
        return std::vector<double>(m_weights.begin() + base, m_weights.begin() + base + m_inputDim);
    }
    template<class DistanceFn>
    std::size_t SelfOrganizingMap<DistanceFn>::getWidth() const noexcept{
        return m_width;
    }
    template<class DistanceFn>
    std::size_t SelfOrganizingMap<DistanceFn>::getHeight() const noexcept{
        return m_height;
    }
    template<class DistanceFn>
    std::size_t SelfOrganizingMap<DistanceFn>::getInputDim() const noexcept{
        return m_inputDim;
    }
    template<class DistanceFn>
    JsonBox::Value SelfOrganizingMap<DistanceFn>::toJson() const{
        JsonBox::Object o;
        o["width"] = std::to_string(m_width);
        o["height"] = std::to_string(m_height);
        o["inputDim"] = std::to_string(m_inputDim);
        o["toroidal"] = JsonBox::Value(m_toroidal);
        JsonBox::Array weightsArr;
        weightsArr.reserve(m_weights.size());
        const double scale = m_weightDecimals ? std::pow(10.0, *m_weightDecimals) : 1.0;
        // A double has a 53 bit significand, so from 2^52 on every one is a whole number already: rounding
        // could only move it by an ulp, and w * scale may not even be finite.
        constexpr double wholeNumbersFrom = static_cast<double>(1ull << (std::numeric_limits<double>::digits - 1));
        for(auto w : m_weights){
            if(m_weightDecimals && std::abs(w * scale) < wholeNumbersFrom){
                w = std::round(w * scale) / scale;
                if(w == 0.0){
                    w = 0.0; // not -0
                }
            }
            weightsArr.push_back(JsonBox::Value(w));
        }
        o["weights"] = weightsArr;
        return o;
    }
    template<class DistanceFn>
    void SelfOrganizingMap<DistanceFn>::setWeightDecimals(std::optional<int> decimals){
        if(decimals && (*decimals < 0 || *decimals > 17)){
            throw std::invalid_argument("SelfOrganizingMap::setWeightDecimals: decimals must be from 0 to 17");
        }
        m_weightDecimals = decimals;
    }
    template<class DistanceFn>
    std::optional<int> SelfOrganizingMap<DistanceFn>::getWeightDecimals() const noexcept{
        return m_weightDecimals;
    }
    template<class DistanceFn>
    std::size_t SelfOrganizingMap<DistanceFn>::idx(std::size_t x, std::size_t y, std::size_t d) const noexcept{
        return (y * m_width + x) * m_inputDim + d;
    }
    template<class DistanceFn>
    double SelfOrganizingMap<DistanceFn>::gridDistance(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const noexcept{
        double dx = static_cast<double>(x1 > x2 ? x1 - x2 : x2 - x1);
        double dy = static_cast<double>(y1 > y2 ? y1 - y2 : y2 - y1);
        if(m_toroidal){
            dx = std::min(dx, static_cast<double>(m_width) - dx);
            dy = std::min(dy, static_cast<double>(m_height) - dy);
        }
        return std::sqrt(dx * dx + dy * dy);
    }
    template<class DistanceFn>
    BestMatchingUnit SelfOrganizingMap<DistanceFn>::findBMU(const std::vector<double>& sample) const noexcept{
        std::size_t bestX = 0u, bestY = 0u;
        double bestDist = std::numeric_limits<double>::max();
        for(std::size_t y=0;y<m_height;++y){
            for(std::size_t x=0;x<m_width;++x){
                auto base = idx(x, y, 0);
                auto d = m_distance(m_weights.data() + base, sample.data(), m_inputDim);
                if(d < bestDist){
                    bestDist = d;
                    bestX = x;
                    bestY = y;
                }
            }
        }
        return {bestX, bestY, bestDist};
    }
    template<class DistanceFn>
    std::pair<BestMatchingUnit, BestMatchingUnit> SelfOrganizingMap<DistanceFn>::findTwoBMUs(const std::vector<double>& sample) const noexcept{
        constexpr auto far = std::numeric_limits<double>::max();
        BestMatchingUnit first{0u, 0u, far};
        BestMatchingUnit second{0u, 0u, far};
        for(std::size_t y=0;y<m_height;++y){
            for(std::size_t x=0;x<m_width;++x){
                auto base = idx(x, y, 0);
                auto d = m_distance(m_weights.data() + base, sample.data(), m_inputDim);
                if(d < first.distance){
                    second = first;
                    first = {x, y, d};
                }else if(d < second.distance){
                    second = {x, y, d};
                }
            }
        }
        if(m_width * m_height == 1u){
            second = first;
        }
        return {first, second};
    }
    template<class DistanceFn>
    bool SelfOrganizingMap<DistanceFn>::adjacent(const BestMatchingUnit& a, const BestMatchingUnit& b, Neighbourhood n) const noexcept{
        auto delta = [this](std::size_t p, std::size_t q, std::size_t size){
            auto d = p > q ? p - q : q - p;
            return m_toroidal ? std::min(d, size - d) : d;
        };
        auto dx = delta(a.x, b.x, m_width);
        auto dy = delta(a.y, b.y, m_height);
        if(n == Neighbourhood::Four){
            return dx + dy == 1u;
        }
        return dx <= 1u && dy <= 1u && dx + dy > 0u;
    }
    template<class DistanceFn>
    std::vector<double> SelfOrganizingMap<DistanceFn>::uMatrix(Neighbourhood neighbours) const{
        std::vector<double> result(m_width * m_height, 0.0);
        std::vector<std::size_t> seen;
        for(std::size_t y=0;y<m_height;++y){
            for(std::size_t x=0;x<m_width;++x){
                const auto* own = m_weights.data() + idx(x, y, 0);
                seen.clear();
                double sum = 0.0;
                for(int dy=-1;dy<=1;++dy){
                    for(int dx=-1;dx<=1;++dx){
                        if((dx == 0 && dy == 0) || (neighbours == Neighbourhood::Four && dx != 0 && dy != 0)){
                            continue;
                        }
                        auto nx = static_cast<long long>(x) + dx;
                        auto ny = static_cast<long long>(y) + dy;
                        if(m_toroidal){
                            nx = (nx + static_cast<long long>(m_width)) % static_cast<long long>(m_width);
                            ny = (ny + static_cast<long long>(m_height)) % static_cast<long long>(m_height);
                        }else if(nx < 0 || ny < 0 || nx >= static_cast<long long>(m_width) || ny >= static_cast<long long>(m_height)){
                            continue;
                        }
                        auto flat = static_cast<std::size_t>(ny) * m_width + static_cast<std::size_t>(nx);
                        if(flat == y * m_width + x || std::find(seen.begin(), seen.end(), flat) != seen.end()){
                            continue;
                        }
                        seen.push_back(flat);
                        sum += m_distance(own, m_weights.data() + idx(static_cast<std::size_t>(nx), static_cast<std::size_t>(ny), 0), m_inputDim);
                    }
                }
                if(!seen.empty()){
                    result[y * m_width + x] = sum / static_cast<double>(seen.size());
                }
            }
        }
        return result;
    }
    template<class DistanceFn>
    SOMEvaluation SelfOrganizingMap<DistanceFn>::evaluate(const std::vector<std::vector<double>>& samples, bool topographicError,
                                                          Neighbourhood adjacency) const{
        SOMEvaluation result;
        result.width = m_width;
        result.height = m_height;
        result.nodeCounts.assign(m_width * m_height, 0u);
        double distanceSum = 0.0;
        std::size_t misplaced = 0u;
        for(const auto& sample : samples){
            if(!validateSample(sample)){
                ++result.samplesSkipped;
                continue;
            }
            ++result.samplesUsed;
            BestMatchingUnit best{0u, 0u, 0.0};
            if(topographicError){
                auto [first, second] = findTwoBMUs(sample);
                best = first;
                if(m_width * m_height > 1u && !adjacent(first, second, adjacency)){
                    ++misplaced;
                }
            }else{
                best = findBMU(sample);
            }
            distanceSum += best.distance;
            ++result.nodeCounts[best.y * m_width + best.x];
        }
        if(result.samplesUsed > 0u){
            auto used = static_cast<double>(result.samplesUsed);
            result.meanQuantizationError = distanceSum / used;
            if(topographicError){
                result.topographicError = static_cast<double>(misplaced) / used;
            }
        }else if(topographicError){
            result.topographicError = 0.0;
        }
        return result;
    }
}

#endif // EVOAI_SELF_ORGANIZING_MAP_HPP
