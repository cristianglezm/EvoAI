#include <EvoAI/Utils/RandomUtils.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    RandomGenerator::RandomGenerator()
    : m_seed(std::chrono::system_clock::now().time_since_epoch().count())
    , m_mtEngine(m_seed){}

    RandomGenerator::RandomGenerator(std::size_t seed)
    : m_seed(seed)
    , m_mtEngine(seed){}

    double RandomGenerator::random(double min, double max, double layerSize) noexcept{
        if(layerSize == 0.0) return random(min,max) * std::sqrt(2.0);
        return random(min, max) * std::sqrt(2.0 / layerSize);
    }
    double RandomGenerator::lecunInit(std::size_t inputSize) noexcept{
        double r = std::sqrt(1.0 / (inputSize ? inputSize:1.0));
        std::uniform_real_distribution<double> ur(-r, r);
        return ur(m_mtEngine);
    }
    double RandomGenerator::lecunInitNormal(std::size_t inputSize) noexcept{
        double r = std::sqrt(1.0 / (inputSize ? inputSize:1.0));
        std::normal_distribution<double> nd(0.0, r);
        auto w = nd(m_mtEngine);
        return std::clamp(w, -r, r);
    }
    double RandomGenerator::heInit(std::size_t inputSize) noexcept{
        auto r = std::sqrt(6.0 / (inputSize ? inputSize: 1.0));
        std::uniform_real_distribution<double> ur(-r, r);
        return ur(m_mtEngine);
    }
    double RandomGenerator::heInitNormal(std::size_t inputSize) noexcept{
        auto r = std::sqrt(2.0 / (inputSize ? inputSize: 1.0));
        std::normal_distribution<double> nd(0.0, r);
        auto w = nd(m_mtEngine);
        return std::clamp(w, -r, r);
    }
    double RandomGenerator::xavierInit(std::size_t inputSize, std::size_t outputSize) noexcept{
        auto sum = inputSize + outputSize;
        double r = std::sqrt(6.0 / (sum ? sum:1.0));
        std::uniform_real_distribution<double> ur(-r, r);
        return ur(m_mtEngine);
    }
    double RandomGenerator::xavierInitNormal(std::size_t inputSize, std::size_t outputSize) noexcept{
        auto sum = inputSize + outputSize;
        double r = std::sqrt(6.0 / (sum ? sum:1.0));
        std::normal_distribution<double> nd(0.0, r);
        auto w = nd(m_mtEngine);
        return std::clamp(w, -r, r);
    }
    bool RandomGenerator::random(float rate) noexcept{
        std::bernoulli_distribution dice(rate);
        return dice(m_mtEngine);
    }
    void RandomGenerator::setSeed(std::size_t seed) noexcept{
        m_seed = seed;
        m_mtEngine.seed(seed);
    }
    RandomGenerator& randomGen() noexcept{
        static auto rg = RandomGenerator(42);
        return rg;
    }
}
