#include <EvoAI/Utils/RandomUtils.hpp>
#include <EvoAI/Utils.hpp>

namespace EvoAI{
    RandomGenerator::RandomGenerator()
    : seed(std::chrono::system_clock::now().time_since_epoch().count())
    , mtEngine(seed){}

    RandomGenerator::RandomGenerator(const unsigned& seed)
    : seed(seed)
    , mtEngine(seed){}

    double RandomGenerator::random(const double& min, const double& max, const double& layerSize) noexcept{
        if(layerSize == 0.0) return random(min,max) * std::sqrt(2);
        return random(min,max) * std::sqrt(2.0/layerSize);
    }

    double RandomGenerator::random(const double& min, const double& max) noexcept{
        std::uniform_real_distribution<double> dice(min, max);
        return dice(mtEngine);
    }

    int RandomGenerator::random(const int& min, const int& max) noexcept{
        std::uniform_int_distribution<int> dice(min, max);
        return dice(mtEngine);
    }
    
    unsigned int RandomGenerator::random(const unsigned int& min, const unsigned int& max) noexcept{
        std::uniform_int_distribution<unsigned int> dice(min, max);
        return dice(mtEngine);
    }
    
    bool RandomGenerator::random(const float& rate) noexcept{
        std::bernoulli_distribution dice(rate);
        return dice(mtEngine);
    }

}