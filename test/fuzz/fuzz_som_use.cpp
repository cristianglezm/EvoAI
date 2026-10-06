// libFuzzer harness for EvoAI::SelfOrganizingMap used through its API with hostile arguments:
// a small map, samples that are the wrong size or hold NaN, infinity, denormals and huge values,
// any learning rate and radius, every scheduler of the library, TrainingOptions (epochOffset, seed,
// onEpoch), evaluate(), uMatrix(), classify(), setWeightDecimals() and a save and load round trip.
// Besides what the sanitizers see, it checks what the documentation promises (counts add up,
// the report names the skipped samples, training in pieces equals one call, the JSON loads back).
#include <EvoAI.hpp>
#include <JsonBox.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <sstream>
#include <vector>

namespace{
    using Map = EvoAI::SelfOrganizingMap<>;

    [[noreturn]] void broken(const char* what){
        std::fprintf(stderr, "invariant broken: %s\n", what);
        std::abort();
    }

    // mostly a value in [0, 1], now and then one of the values that break arithmetic
    double anyDouble(FuzzedDataProvider& in){
        switch(in.ConsumeIntegralInRange<int>(0, 15)){
            case 0: return std::numeric_limits<double>::quiet_NaN();
            case 1: return std::numeric_limits<double>::infinity();
            case 2: return -std::numeric_limits<double>::infinity();
            case 3: return 0.0;
            case 4: return -0.0;
            case 5: return std::numeric_limits<double>::denorm_min();
            case 6: return std::numeric_limits<double>::max();
            case 7: return -std::numeric_limits<double>::max();
            case 8: return in.ConsumeFloatingPoint<double>();
            default: return in.ConsumeProbability<double>();
        }
    }

    std::vector<std::vector<double>> someSamples(FuzzedDataProvider& in, std::size_t dim){
        std::vector<std::vector<double>> samples;
        auto count = in.ConsumeIntegralInRange<std::size_t>(0, 12);
        for(std::size_t i=0;i<count;++i){
            auto size = in.ConsumeIntegralInRange<std::size_t>(0, 7) == 0 ? in.ConsumeIntegralInRange<std::size_t>(0, dim + 2) : dim;
            std::vector<double> sample;
            for(std::size_t d=0;d<size;++d){
                sample.push_back(anyDouble(in));
            }
            samples.push_back(std::move(sample));
        }
        return samples;
    }

    std::vector<std::uint64_t> bitsOfWeights(const Map& som){
        std::vector<std::uint64_t> bits;
        for(std::size_t y=0;y<som.getHeight();++y){
            for(std::size_t x=0;x<som.getWidth();++x){
                for(auto w : som.getPrototype(x, y)){
                    std::uint64_t b;
                    std::memcpy(&b, &w, sizeof b);
                    bits.push_back(b);
                }
            }
        }
        return bits;
    }

    bool allFinite(const Map& som){
        for(std::size_t y=0;y<som.getHeight();++y){
            for(std::size_t x=0;x<som.getWidth();++x){
                for(auto w : som.getPrototype(x, y)){
                    if(!std::isfinite(w)){
                        return false;
                    }
                }
            }
        }
        return true;
    }

    // A whole number from 2^31 to 1e17 is written without a point and JsonBox reads it back as an int that
    // saturates at 2^31 - 1: a limit of its parser, not something the map can do anything about.
    bool hasWholeWeightJsonBoxCannotCarry(const Map& som){
        for(std::size_t y=0;y<som.getHeight();++y){
            for(std::size_t x=0;x<som.getWidth();++x){
                for(auto w : som.getPrototype(x, y)){
                    if(std::abs(w) >= 2147483648.0 && std::abs(w) < 1e17 && std::floor(w) == w){
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // one of the schedulers of the library, with parameters from the input, handed to f
    template<class F>
    void withScheduler(FuzzedDataProvider& in, F&& f){
        switch(in.ConsumeIntegralInRange<int>(0, 5)){
            case 0: f(EvoAI::Scheduler<EvoAI::ConstantLR>(EvoAI::ConstantLR{})); break;
            case 1: f(EvoAI::Scheduler<EvoAI::ExponentialLR>(EvoAI::ExponentialLR{anyDouble(in)})); break;
            case 2: f(EvoAI::Scheduler<EvoAI::MultiplicativeLR>(EvoAI::MultiplicativeLR{anyDouble(in)})); break;
            case 3: {
                auto step = in.ConsumeIntegralInRange<std::size_t>(0, 4);
                f(EvoAI::Scheduler<EvoAI::StepLR>(EvoAI::StepLR{step, anyDouble(in)}));
                break;
            }
            case 4: {
                auto step = in.ConsumeIntegralInRange<std::size_t>(0, 4);
                auto decrement = anyDouble(in);
                f(EvoAI::Scheduler<EvoAI::LinearLR>(EvoAI::LinearLR{step, decrement, anyDouble(in)}));
                break;
            }
            default: {
                std::vector<std::size_t> steps;
                for(auto n = in.ConsumeIntegralInRange<int>(0, 4); n > 0; --n){
                    steps.push_back(in.ConsumeIntegralInRange<std::size_t>(0, 8));
                }
                f(EvoAI::Scheduler<EvoAI::MultiStepLR>(EvoAI::MultiStepLR{std::move(steps), anyDouble(in)}));
                break;
            }
        }
    }

    void checkEvaluation(const Map& som, const std::vector<std::vector<double>>& samples){
        std::size_t invalid = 0;
        std::vector<std::size_t> counts(som.getWidth() * som.getHeight(), 0u);
        for(const auto& sample : samples){
            auto match = som.classify(sample);
            if(som.validateSample(sample) != match.has_value()){
                broken("classify() and validateSample() disagree");
            }
            if(!match){
                ++invalid;
                continue;
            }
            if(match->x >= som.getWidth() || match->y >= som.getHeight() || std::isnan(match->distance)){
                broken("classify() gave a node outside the grid or a NaN distance");
            }
            ++counts[match->y * som.getWidth() + match->x];
        }
        auto plain = som.evaluate(samples);
        auto topo = som.evaluate(samples, true, EvoAI::Neighbourhood::Four);
        auto topoEight = som.evaluate(samples, true, EvoAI::Neighbourhood::Eight);
        for(const auto* e : {&plain, &topo, &topoEight}){
            std::size_t sum = 0;
            for(auto c : e->nodeCounts){
                sum += c;
            }
            if(e->nodeCounts != counts || sum != e->samplesUsed || e->samplesSkipped != invalid
               || e->samplesUsed + e->samplesSkipped != samples.size()){
                broken("evaluate() does not add up or disagrees with classify()");
            }
            if(e->usedNodes() > e->nodeCounts.size() || e->busiestNodeCount() > e->samplesUsed
               || !(e->fractionUsed() >= 0.0 && e->fractionUsed() <= 1.0)){
                broken("evaluate() helpers out of range");
            }
        }
        if(plain.topographicError || !topo.topographicError || !topoEight.topographicError
           || !(*topo.topographicError >= 0.0 && *topo.topographicError <= 1.0)
           || !(*topoEight.topographicError >= 0.0 && *topoEight.topographicError <= 1.0)
           // diagonals only add neighbours, so fewer samples can be badly placed with Eight
           || *topoEight.topographicError > *topo.topographicError){
            broken("topographic error missing, out of [0, 1], or Eight worse than Four");
        }
        std::uint64_t a, b;
        std::memcpy(&a, &plain.meanQuantizationError, sizeof a);
        std::memcpy(&b, &topo.meanQuantizationError, sizeof b);
        if(a != b){
            broken("the topographic flag changed the quantization error");
        }
        for(auto n : {EvoAI::Neighbourhood::Four, EvoAI::Neighbourhood::Eight}){
            if(som.uMatrix(n).size() != som.getWidth() * som.getHeight()){
                broken("uMatrix() has the wrong size");
            }
        }
    }
}

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size){
    FuzzedDataProvider in(data, size);
    try{
        EvoAI::randomGen().setSeed(in.ConsumeIntegral<std::uint32_t>());
        auto width = in.ConsumeIntegralInRange<std::size_t>(1, 6);
        auto height = in.ConsumeIntegralInRange<std::size_t>(1, 6);
        auto dim = in.ConsumeIntegralInRange<std::size_t>(1, 6);
        Map som(width, height, dim, in.ConsumeBool());
        auto samples = someSamples(in, dim);

        // what the map says about hostile samples, before it has been trained
        checkEvaluation(som, samples);

        // training: the report is checked, then the same training in pieces
        auto epochs = in.ConsumeIntegralInRange<std::size_t>(0, 6);
        auto baseLR = anyDouble(in);
        auto baseRadius = anyDouble(in);
        EvoAI::TrainingOptions options;
        options.epochOffset = in.ConsumeIntegralInRange<std::size_t>(0, 64);
        if(in.ConsumeBool()){
            options.seed = in.ConsumeIntegral<std::uint64_t>();
        }
        auto pieces = in.ConsumeIntegralInRange<std::size_t>(0, epochs);
        auto stopAfter = in.ConsumeIntegralInRange<std::size_t>(0, 8);
        auto lrParameters = in.ConsumeBytes<std::uint8_t>(40);  // a scheduler is built from the same bytes for every run
        auto radiusParameters = in.ConsumeBytes<std::uint8_t>(40);
        auto remaining = in.ConsumeRemainingBytes<std::uint8_t>();

        std::vector<std::size_t> invalidIndices;
        for(std::size_t i=0;i<samples.size();++i){
            if(!som.validateSample(samples[i])){
                invalidIndices.push_back(i);
            }
        }
        auto run = [&](Map& map, std::size_t count, const EvoAI::TrainingOptions& opts){
            EvoAI::TrainingReport report;
            FuzzedDataProvider lr(lrParameters.data(), lrParameters.size());
            FuzzedDataProvider radius(radiusParameters.data(), radiusParameters.size());
            withScheduler(lr, [&](auto lrSchedule){
                withScheduler(radius, [&](auto radiusSchedule){
                    report = map.train(samples, count, baseLR, baseRadius, std::move(lrSchedule), std::move(radiusSchedule), opts);
                });
            });
            return report;
        };
        Map whole = som;
        Map inPieces = som;
        auto report = run(whole, epochs, options);
        if(report.skipped != invalidIndices || report.nodeCounts.size() != width * height || report.epochs.size() > epochs
           || (report.epochs.size() != epochs && invalidIndices.size() != samples.size())){
            broken("the training report is wrong");
        }
        for(std::size_t i=0;i<report.epochs.size();++i){
            if(report.epochs[i].epoch != options.epochOffset + i){
                broken("an epoch has the wrong index");
            }
        }
        if(options.seed){
            auto first = run(inPieces, pieces, options);
            auto rest = options;
            rest.epochOffset += pieces;
            auto second = run(inPieces, epochs - pieces, rest);
            if(bitsOfWeights(whole) != bitsOfWeights(inPieces)){
                broken("training in pieces gave other weights than one call");
            }
            if(first.epochs.size() + second.epochs.size() != report.epochs.size()){
                broken("pieces ran another number of epochs");
            }
        }
        // stopping from the callback
        Map stopped = som;
        auto stopOptions = options;
        std::size_t calls = 0;
        stopOptions.onEpoch = [&](const EvoAI::EpochInfo&){
            ++calls;
            return calls <= stopAfter;
        };
        auto stoppedReport = run(stopped, epochs, stopOptions);
        if(stoppedReport.epochs.size() != calls || calls > std::min(epochs, stopAfter + 1)){
            broken("onEpoch did not stop the training where it said");
        }

        // the trained map against hostile samples again
        checkEvaluation(whole, samples);

        // save and load, exact or rounded
        std::optional<int> decimals;
        if(in.ConsumeBool() || !remaining.empty()){
            decimals = static_cast<int>(remaining.empty() ? 4 : static_cast<int>(remaining[0] % 24) - 3);
        }
        bool valid = !decimals || (*decimals >= 0 && *decimals <= 17);
        try{
            whole.setWeightDecimals(decimals);
            if(!valid){
                broken("setWeightDecimals() accepted a value outside 0 to 17");
            }
        }catch(const std::invalid_argument&){
            if(valid){
                broken("setWeightDecimals() rejected a valid value");
            }
            decimals.reset();
        }
        if(whole.getWeightDecimals() != decimals){
            broken("getWeightDecimals() does not say what was set");
        }
        if(allFinite(whole) && !hasWholeWeightJsonBoxCannotCarry(whole)){
            std::stringstream text;
            whole.toJson().writeToStream(text, false, false);
            JsonBox::Value parsed;
            parsed.loadFromString(text.str());
            Map loaded(parsed.getObject());
            if(loaded.getWidth() != width || loaded.getHeight() != height || loaded.getInputDim() != dim){
                broken("a saved map loaded with another size");
            }
            if(!decimals){
                if(bitsOfWeights(loaded) != bitsOfWeights(whole)){
                    broken("an exact save and load changed a weight");
                }
            }else{
                auto scale = std::pow(10.0, *decimals);
                for(std::size_t y=0;y<height;++y){
                    for(std::size_t x=0;x<width;++x){
                        auto before = whole.getPrototype(x, y);
                        auto after = loaded.getPrototype(x, y);
                        for(std::size_t d=0;d<dim;++d){
                            // half a unit of the last decimal, plus the rounding error of the doubles themselves
                            auto allowed = 0.5 / scale + std::abs(before[d]) * 4.0 * std::numeric_limits<double>::epsilon();
                            if(!std::isfinite(after[d]) || std::abs(after[d] - before[d]) > allowed){
                                std::fprintf(stderr, "weight %.17g loaded as %.17g with %d decimals\n", before[d], after[d], *decimals);
                                broken("a rounded save and load moved a weight by more than the rounding");
                            }
                        }
                    }
                }
            }
        }
    }catch(const std::exception&){
        // a constructor that refuses its arguments is fine: the sanitizers are the judge of the rest
    }
    return 0;
}
