#ifndef EVOAI_SELF_ORGANIZING_MAP_TEST_HPP
#define EVOAI_SELF_ORGANIZING_MAP_TEST_HPP

#include <atomic>
#include <limits>
#include <thread>

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        namespace{
            std::vector<double> g_lrLog;
            std::vector<double> g_radiusLog;
            struct LoggingLR{
                LoggingLR() = default;
                LoggingLR(JsonBox::Object){}
                JsonBox::Value toJson() const noexcept{ return JsonBox::Value(JsonBox::Object{}); }
                double operator()(double lr, std::size_t) const noexcept{
                    g_lrLog.push_back(lr);
                    return lr - 1.0;
                }
            };
            struct LoggingRadius{
                LoggingRadius() = default;
                LoggingRadius(JsonBox::Object){}
                JsonBox::Value toJson() const noexcept{ return JsonBox::Value(JsonBox::Object{}); }
                double operator()(double radius, std::size_t) const noexcept{
                    g_radiusLog.push_back(radius);
                    return radius - 1.0;
                }
            };
        }
        static_assert(meta::distance_metric_v<EuclideanDistance>, "EuclideanDistance must fulfill meta::distance_metric_v");
        // struct NotADistance{};
        // compilation error as intended (NotADistance does not fulfill meta::distance_metric_v requirements)
        // SelfOrganizingMap<NotADistance> somDummy(1,1,1);

        TEST(SelfOrganizingMapTest, EuclideanDistance){
            auto dist = EuclideanDistance{};
            const std::vector<double> a = {1.0, 2.0, 3.0};
            const std::vector<double> origin = {0.0, 0.0};
            const std::vector<double> p345 = {3.0, 4.0};
            EXPECT_DOUBLE_EQ(0.0, dist(a.data(), a.data(), a.size()));
            EXPECT_DOUBLE_EQ(5.0, dist(origin.data(), p345.data(), 2u));
            EXPECT_DOUBLE_EQ(1.0, dist(a.data(), origin.data(), 1u)); // only the first n values count
        }
        TEST(SelfOrganizingMapTest, ConstructionThrowsOnZeroDim){
            EXPECT_THROW(SelfOrganizingMap<>(0u, 5u, 3u), std::invalid_argument);
            EXPECT_THROW(SelfOrganizingMap<>(5u, 0u, 3u), std::invalid_argument);
            EXPECT_THROW(SelfOrganizingMap<>(5u, 5u, 0u), std::invalid_argument);
            EXPECT_NO_THROW(SelfOrganizingMap<>(5u, 5u, 3u));
        }
        TEST(SelfOrganizingMapTest, GettersMatchConstructorArgs){
            SelfOrganizingMap<> som(4u, 3u, 5u);
            EXPECT_EQ(4u, som.getWidth());
            EXPECT_EQ(3u, som.getHeight());
            EXPECT_EQ(5u, som.getInputDim());
        }
        TEST(SelfOrganizingMapTest, ValidateSample){
            SelfOrganizingMap<> som(4u, 4u, 3u);
            EXPECT_TRUE(som.validateSample({1.0, 2.0, 3.0}));
            EXPECT_FALSE(som.validateSample({1.0, 2.0})); // wrong size
            EXPECT_FALSE(som.validateSample({1.0, 2.0, 3.0, 4.0})); // wrong size
            EXPECT_FALSE(som.validateSample({1.0, std::numeric_limits<double>::quiet_NaN(), 3.0})); // NaN
            EXPECT_FALSE(som.validateSample({1.0, std::numeric_limits<double>::infinity(), 3.0})); // Inf
        }
        TEST(SelfOrganizingMapTest, GetPrototypeSizeAndBounds){
            SelfOrganizingMap<> som(4u, 3u, 6u);
            auto proto = som.getPrototype(2u, 1u);
            EXPECT_EQ(6u, proto.size());
            for(auto v : proto){
                EXPECT_GE(v, 0.0);
                EXPECT_LE(v, 1.0);
            }
            EXPECT_THROW(som.getPrototype(4u, 0u), std::out_of_range); // x == width
            EXPECT_THROW(som.getPrototype(0u, 3u), std::out_of_range); // y == height
        }
        TEST(SelfOrganizingMapTest, ClassifyReturnsNulloptForInvalidSample){
            SelfOrganizingMap<> som(4u, 4u, 3u);
            EXPECT_FALSE(som.classify({1.0, 2.0}).has_value()); // wrong size
            EXPECT_FALSE(som.classify({1.0, std::numeric_limits<double>::quiet_NaN(), 3.0}).has_value()); // NaN
            EXPECT_TRUE(som.classify({1.0, 2.0, 3.0}).has_value());
        }
        namespace{
            // 2x2 grid, 2 features: (0,0)=[0,0] (1,0)=[1,0] (0,1)=[0,1] (1,1)=[1,1]
            SelfOrganizingMap<> cornersMap(){
                JsonBox::Object o;
                o["width"] = JsonBox::Value(std::string("2"));
                o["height"] = JsonBox::Value(std::string("2"));
                o["inputDim"] = JsonBox::Value(std::string("2"));
                o["toroidal"] = JsonBox::Value(false);
                JsonBox::Array w;
                for(auto v : {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0}){
                    w.push_back(JsonBox::Value(v));
                }
                o["weights"] = JsonBox::Value(w);
                return SelfOrganizingMap<>(o);
            }
        }
        TEST(SelfOrganizingMapTest, ClassifyReturnsTheWinnerAndItsDistance){
            auto som = cornersMap();
            auto bmu = som.classify({0.9, 0.1});
            ASSERT_TRUE(bmu.has_value());
            EXPECT_EQ(1u, bmu->x);
            EXPECT_EQ(0u, bmu->y);
            EXPECT_DOUBLE_EQ(std::sqrt(0.1 * 0.1 + 0.1 * 0.1), bmu->distance);
            const std::vector<double> sample = {0.9, 0.1};
            const auto prototype = som.getPrototype(bmu->x, bmu->y);
            EXPECT_DOUBLE_EQ(EuclideanDistance{}(prototype.data(), sample.data(), 2u), bmu->distance);
        }
        TEST(SelfOrganizingMapTest, ClassifyOfAPrototypeIsItsNodeAtDistanceZero){
            auto som = cornersMap();
            for(std::size_t y = 0; y < som.getHeight(); ++y){
                for(std::size_t x = 0; x < som.getWidth(); ++x){
                    auto bmu = som.classify(som.getPrototype(x, y));
                    ASSERT_TRUE(bmu.has_value());
                    EXPECT_EQ(x, bmu->x);
                    EXPECT_EQ(y, bmu->y);
                    EXPECT_EQ(0.0, bmu->distance);
                }
            }
        }
        TEST(SelfOrganizingMapTest, ClassifyTieGoesToTheFirstNodeInRowMajorOrder){
            auto som = cornersMap();
            // equally far (0.5) from (0,0) and (1,0), and from (0,1) and (1,1)
            auto top = som.classify({0.5, 0.0});
            ASSERT_TRUE(top.has_value());
            EXPECT_EQ(0u, top->x);
            EXPECT_EQ(0u, top->y);
            auto bottom = som.classify({0.5, 1.0});
            ASSERT_TRUE(bottom.has_value());
            EXPECT_EQ(0u, bottom->x);
            EXPECT_EQ(1u, bottom->y);
            // the centre is equally far from all four
            auto centre = som.classify({0.5, 0.5});
            ASSERT_TRUE(centre.has_value());
            EXPECT_EQ(0u, centre->x);
            EXPECT_EQ(0u, centre->y);
        }
        namespace{
            template<class DistanceFn = EuclideanDistance>
            SelfOrganizingMap<DistanceFn> mapWith(std::size_t width, std::size_t height, std::size_t dim, const std::vector<double>& weights, bool toroidal){
                JsonBox::Object o;
                o["width"] = JsonBox::Value(std::to_string(width));
                o["height"] = JsonBox::Value(std::to_string(height));
                o["inputDim"] = JsonBox::Value(std::to_string(dim));
                o["toroidal"] = JsonBox::Value(toroidal);
                JsonBox::Array w;
                for(auto v : weights){
                    w.push_back(JsonBox::Value(v));
                }
                o["weights"] = JsonBox::Value(w);
                return SelfOrganizingMap<DistanceFn>(o);
            }
            // sum of absolute differences
            struct ManhattanDistance final{
                double operator()(const double* a, const double* b, std::size_t n) const noexcept{
                    double sum = 0.0;
                    for(std::size_t i=0;i<n;++i){
                        sum += std::abs(a[i] - b[i]);
                    }
                    return sum;
                }
            };
        }
        static_assert(meta::distance_metric_v<ManhattanDistance>, "a metric over pointers must fulfill meta::distance_metric_v");
        TEST(SelfOrganizingMapTest, AMapUsesTheDistanceItIsGiven){
            auto som = mapWith<ManhattanDistance>(2u, 1u, 2u, {0.0, 0.0, 1.0, 1.0}, false);
            auto bmu = som.classify({0.4, 0.4});
            ASSERT_TRUE(bmu.has_value());
            EXPECT_EQ(0u, bmu->x);
            EXPECT_DOUBLE_EQ(0.8, bmu->distance); // 0.4 + 0.4, not the euclidean 0.57
            auto e = som.evaluate({{0.4, 0.4}, {0.9, 0.9}});
            EXPECT_DOUBLE_EQ((0.8 + 0.2) / 2.0, e.meanQuantizationError);
            EXPECT_DOUBLE_EQ(2.0, som.uMatrix()[0]); // the two prototypes are 2 apart
        }
#ifndef __EMSCRIPTEN__
        TEST(SelfOrganizingMapTest, ClassifyFromSeveralThreadsGivesTheSameAnswers){
            randomGen().setSeed(2u);
            SelfOrganizingMap<> som(8u, 8u, 64u);
            std::vector<std::vector<double>> samples;
            for(auto i=0u;i<64u;++i){
                std::vector<double> v;
                for(auto d=0u;d<64u;++d){
                    v.push_back(randomGen().random(0.0, 1.0));
                }
                samples.push_back(v);
            }
            std::vector<std::pair<std::size_t, std::size_t>> expected;
            for(const auto& sample : samples){
                auto bmu = som.classify(sample);
                expected.emplace_back(bmu->x, bmu->y);
            }
            std::atomic<int> wrong{0};
            std::vector<std::thread> threads;
            for(auto t=0u;t<4u;++t){
                threads.emplace_back([&]{
                    for(auto round=0u;round<100u;++round){
                        for(std::size_t i=0;i<samples.size();++i){
                            auto bmu = som.classify(samples[i]);
                            if(bmu->x != expected[i].first || bmu->y != expected[i].second){
                                ++wrong;
                            }
                        }
                    }
                });
            }
            for(auto& t : threads){
                t.join();
            }
            EXPECT_EQ(0, wrong.load());
        }
#endif
        TEST(SelfOrganizingMapTest, EvaluateCountsDistancesAndSkippedSamples){
            auto som = cornersMap();
            auto e = som.evaluate({{0.0, 0.0}, {0.9, 0.1}, {1.0, 1.0}, {1.0}, {0.0, std::numeric_limits<double>::quiet_NaN()}});
            EXPECT_EQ(2u, e.width);
            EXPECT_EQ(2u, e.height);
            EXPECT_EQ(3u, e.samplesUsed);
            EXPECT_EQ(2u, e.samplesSkipped);
            EXPECT_NEAR((0.0 + std::sqrt(0.02) + 0.0) / 3.0, e.meanQuantizationError, 1e-15);
            EXPECT_EQ((std::vector<std::size_t>{1u, 1u, 0u, 1u}), e.nodeCounts); // y * width + x
            EXPECT_EQ(3u, e.usedNodes());
            EXPECT_EQ(1u, e.busiestNodeCount());
            EXPECT_DOUBLE_EQ(0.75, e.fractionUsed());
            EXPECT_FALSE(e.topographicError.has_value());
        }
        TEST(SelfOrganizingMapTest, EvaluateBusiestNodeIsTheMaximum){
            auto e = cornersMap().evaluate({{0.0, 0.0}, {0.1, 0.0}, {0.05, 0.05}, {1.0, 1.0}});
            EXPECT_EQ((std::vector<std::size_t>{3u, 0u, 0u, 1u}), e.nodeCounts);
            EXPECT_EQ(3u, e.busiestNodeCount());
            EXPECT_EQ(2u, e.usedNodes());
            EXPECT_DOUBLE_EQ(0.5, e.fractionUsed());
        }
        TEST(SelfOrganizingMapTest, EvaluateWithNoUsableSamples){
            auto som = cornersMap();
            auto e = som.evaluate({});
            EXPECT_EQ(0u, e.samplesUsed);
            EXPECT_EQ(0.0, e.meanQuantizationError);
            EXPECT_EQ(4u, e.nodeCounts.size());
            EXPECT_EQ(0u, e.usedNodes());
            EXPECT_EQ(0u, e.busiestNodeCount());
            EXPECT_EQ(0.0, e.fractionUsed());
            auto t = som.evaluate({{1.0}}, true);
            EXPECT_EQ(1u, t.samplesSkipped);
            ASSERT_TRUE(t.topographicError.has_value());
            EXPECT_EQ(0.0, *t.topographicError);
        }
        TEST(SelfOrganizingMapTest, EvaluateAgreesWithClassifyAndThePrototypeDistance){
            randomGen().setSeed(7u);
            SelfOrganizingMap<> som(4u, 3u, 5u);
            std::vector<std::vector<double>> samples;
            for(auto i=0u;i<60u;++i){
                std::vector<double> v;
                for(auto d=0u;d<5u;++d){
                    v.push_back(randomGen().random(0.0, 1.0));
                }
                samples.push_back(v);
            }
            som.train(samples, 20u, 0.5, 2.0, Scheduler<ExponentialLR>(ExponentialLR{0.9}), Scheduler<ExponentialLR>(ExponentialLR{0.9}));
            std::vector<std::size_t> counts(12u, 0u);
            double sum = 0.0;
            for(const auto& sample : samples){
                auto bmu = som.classify(sample);
                ASSERT_TRUE(bmu.has_value());
                ++counts[bmu->y * 4u + bmu->x];
                const auto prototype = som.getPrototype(bmu->x, bmu->y);
                sum += EuclideanDistance{}(prototype.data(), sample.data(), prototype.size());
            }
            auto e = som.evaluate(samples);
            EXPECT_EQ(counts, e.nodeCounts);
            EXPECT_NEAR(sum / 60.0, e.meanQuantizationError, 1e-12);
            EXPECT_EQ(60u, e.samplesUsed);
        }
        TEST(SelfOrganizingMapTest, TopographicErrorCountsSamplesWhoseTwoClosestNodesAreFarApart){
            // nodes in a row hold 0, 10 and 1: the nodes holding 0 and 1 are two cells apart
            auto som = mapWith(3u, 1u, 1u, {0.0, 10.0, 1.0}, false);
            auto e = som.evaluate({{0.4}, {9.0}}, true);
            ASSERT_TRUE(e.topographicError.has_value());
            // 0.4: best (0,0), second (2,0) -> apart. 9.0: best (1,0), second (2,0) -> neighbours
            EXPECT_DOUBLE_EQ(0.5, *e.topographicError);
            EXPECT_DOUBLE_EQ((0.4 + 1.0) / 2.0, e.meanQuantizationError);
        }
        TEST(SelfOrganizingMapTest, TopographicErrorDiagonalsDependOnTheNeighbourhood){
            // (0,0) holds 0 and the diagonal node (1,1) holds 0.2, the other two are far away
            auto som = mapWith(2u, 2u, 1u, {0.0, 5.0, 5.0, 0.2}, false);
            auto eight = som.evaluate({{0.05}}, true, Neighbourhood::Eight);
            auto four = som.evaluate({{0.05}}, true, Neighbourhood::Four);
            EXPECT_EQ(0.0, *eight.topographicError);
            EXPECT_EQ(1.0, *four.topographicError);
        }
        TEST(SelfOrganizingMapTest, TopographicErrorWrapsAroundWhenToroidal){
            // the nodes holding 0 and 0.1 are the two ends of the row
            std::vector<double> w = {0.0, 5.0, 6.0, 0.1};
            auto flat = mapWith(4u, 1u, 1u, w, false);
            auto torus = mapWith(4u, 1u, 1u, w, true);
            EXPECT_EQ(1.0, *flat.evaluate({{0.02}}, true, Neighbourhood::Four).topographicError);
            EXPECT_EQ(0.0, *torus.evaluate({{0.02}}, true, Neighbourhood::Four).topographicError);
        }
        TEST(SelfOrganizingMapTest, TopographicErrorOfAOneNodeMapIsZero){
            auto som = mapWith(1u, 1u, 1u, {0.5}, true);
            auto e = som.evaluate({{0.1}, {0.9}}, true);
            EXPECT_EQ(0.0, *e.topographicError);
            EXPECT_EQ(1u, e.usedNodes());
        }
        TEST(SelfOrganizingMapTest, UMatrixIsTheMeanDistanceToTheNeighbours){
            auto row = mapWith(3u, 1u, 1u, {0.0, 1.0, 4.0}, false);
            auto u = row.uMatrix(Neighbourhood::Four);
            ASSERT_EQ(3u, u.size());
            EXPECT_DOUBLE_EQ(1.0, u[0]);       // |0-1|
            EXPECT_DOUBLE_EQ(2.0, u[1]);       // (|1-0| + |1-4|) / 2
            EXPECT_DOUBLE_EQ(3.0, u[2]);       // |4-1|
            EXPECT_EQ(u, row.uMatrix(Neighbourhood::Eight)); // a single row has no diagonals
        }
        TEST(SelfOrganizingMapTest, UMatrixIsIndexedByYTimesWidthPlusX){
            auto som = mapWith(3u, 2u, 1u, {0.0, 1.0, 2.0, 10.0, 11.0, 12.0}, false);
            auto u = som.uMatrix(Neighbourhood::Four);
            ASSERT_EQ(6u, u.size());
            EXPECT_DOUBLE_EQ(5.5, u[0]);       // (|0-1| + |0-10|) / 2
            EXPECT_DOUBLE_EQ(4.0, u[1]);       // (1 + 1 + 10) / 3
            EXPECT_DOUBLE_EQ(5.5, u[5]);       // (|12-11| + |12-2|) / 2
        }
        TEST(SelfOrganizingMapTest, UMatrixDiagonalsDependOnTheNeighbourhood){
            auto som = cornersMap();
            auto four = som.uMatrix(Neighbourhood::Four);
            auto eight = som.uMatrix(Neighbourhood::Eight);
            for(auto v : four){
                EXPECT_DOUBLE_EQ(1.0, v);
            }
            for(auto v : eight){
                EXPECT_DOUBLE_EQ((1.0 + 1.0 + std::sqrt(2.0)) / 3.0, v);
            }
        }
        TEST(SelfOrganizingMapTest, UMatrixWrapsAroundWhenToroidal){
            auto flat = mapWith(4u, 1u, 1u, {0.0, 1.0, 2.0, 3.0}, false);
            auto torus = mapWith(4u, 1u, 1u, {0.0, 1.0, 2.0, 3.0}, true);
            EXPECT_DOUBLE_EQ(1.0, flat.uMatrix(Neighbourhood::Four)[0]);
            EXPECT_DOUBLE_EQ(2.0, torus.uMatrix(Neighbourhood::Four)[0]); // (|0-1| + |0-3|) / 2
        }
        TEST(SelfOrganizingMapTest, UMatrixCountsAWrappedNeighbourOnce){
            // 2 wide and toroidal: the left and the right neighbour are the same node
            auto thin = mapWith(2u, 1u, 1u, {0.0, 10.0}, true);
            EXPECT_EQ((std::vector<double>{10.0, 10.0}), thin.uMatrix(Neighbourhood::Four));
            // 2x2 toroidal wraps every diagonal onto the same node: same result as without wrapping
            std::vector<double> w = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
            auto torus = mapWith(2u, 2u, 2u, w, true);
            auto flat = mapWith(2u, 2u, 2u, w, false);
            auto a = torus.uMatrix(Neighbourhood::Eight);
            auto b = flat.uMatrix(Neighbourhood::Eight);
            for(auto i=0u;i<4u;++i){
                EXPECT_DOUBLE_EQ(b[i], a[i]);
            }
        }
        TEST(SelfOrganizingMapTest, UMatrixOfASingleNodeIsZero){
            EXPECT_EQ((std::vector<double>{0.0}), mapWith(1u, 1u, 1u, {0.5}, true).uMatrix());
            EXPECT_EQ((std::vector<double>{0.0}), mapWith(1u, 1u, 1u, {0.5}, false).uMatrix());
        }
        namespace{
            std::vector<std::vector<double>> randomSamples(std::size_t count, std::size_t dim){
                std::vector<std::vector<double>> samples;
                for(std::size_t i=0;i<count;++i){
                    std::vector<double> v;
                    for(std::size_t d=0;d<dim;++d){
                        v.push_back(randomGen().random(0.0, 1.0));
                    }
                    samples.push_back(v);
                }
                return samples;
            }
            std::vector<double> allWeights(const SelfOrganizingMap<>& som){
                std::vector<double> all;
                for(std::size_t y=0;y<som.getHeight();++y){
                    for(std::size_t x=0;x<som.getWidth();++x){
                        auto p = som.getPrototype(x, y);
                        all.insert(all.end(), p.begin(), p.end());
                    }
                }
                return all;
            }
        }
        namespace{
            std::string jsonText(const SelfOrganizingMap<>& som){
                std::stringstream ss;
                som.toJson().writeToStream(ss, false, false);
                return ss.str();
            }
            // setWeightDecimals() only makes the JSON shorter with a JsonBox that writes a double with the shortest
            // text that reads back exactly (0.5489, not 0.54890000000000005)
            std::string textOfDouble(double value){
                std::stringstream ss;
                JsonBox::Value(value).writeToStream(ss, false, false);
                return ss.str();
            }
            constexpr const char* needsShortDoubles = "needs a JsonBox that writes the shortest text of a double (feat(Value): write doubles with the shortest text that reads back exactly)";
            SelfOrganizingMap<> fromText(const std::string& text){
                JsonBox::Value v;
                v.loadFromString(text);
                return SelfOrganizingMap<>(v.getObject());
            }
        }
        TEST(SelfOrganizingMapTest, WeightsAreWrittenExactlyByDefault){
            randomGen().setSeed(4u);
            SelfOrganizingMap<> som(3u, 3u, 6u);
            EXPECT_FALSE(som.getWeightDecimals().has_value());
            auto loaded = fromText(jsonText(som));
            EXPECT_EQ(allWeights(som), allWeights(loaded));
        }
        TEST(SelfOrganizingMapTest, WeightDecimalsRoundTheJsonAndSnapToZeroAndOne){
            ASSERT_EQ("0.5489", textOfDouble(0.5489)) << needsShortDoubles;
            auto som = mapWith(5u, 1u, 1u, {0.99999, 0.00003, -0.00001, 0.123456, 0.55555}, false);
            som.setWeightDecimals(4);
            auto text = jsonText(som);
            EXPECT_EQ(std::string::npos, text.find("-0"));
            auto loaded = fromText(text);
            auto w = allWeights(loaded);
            ASSERT_EQ(5u, w.size());
            EXPECT_EQ(1.0, w[0]);
            EXPECT_EQ(0.0, w[1]);
            EXPECT_EQ(0.0, w[2]);
            EXPECT_EQ(0.1235, w[3]);
            EXPECT_EQ(0.5556, w[4]); // 0.55555 is a hair above in binary
            EXPECT_NE(std::string::npos, text.find("0.1235"));
            // the map itself keeps what it had
            EXPECT_EQ(0.99999, som.getPrototype(0u, 0u)[0]);
            EXPECT_EQ(0.123456, som.getPrototype(3u, 0u)[0]);
            // and a loaded map writes exactly again
            EXPECT_FALSE(loaded.getWeightDecimals().has_value());
        }
        TEST(SelfOrganizingMapTest, WeightDecimalsMakeTheJsonMuchShorter){
            ASSERT_EQ("0.5489", textOfDouble(0.5489)) << needsShortDoubles;
            randomGen().setSeed(6u);
            SelfOrganizingMap<> som(8u, 8u, 32u);
            auto exact = jsonText(som);
            som.setWeightDecimals(4);
            auto rounded = jsonText(som);
            EXPECT_LT(rounded.size() * 2u, exact.size());
            auto a = allWeights(som);
            auto b = allWeights(fromText(rounded));
            ASSERT_EQ(a.size(), b.size());
            for(std::size_t i=0;i<a.size();++i){
                ASSERT_NEAR(a[i], b[i], 5.0e-5 + 1e-12);
            }
        }
        TEST(SelfOrganizingMapTest, WeightDecimalsLeaveWeightsAloneThatRoundingCannotChange){
            ASSERT_EQ("0.5489", textOfDouble(0.5489)) << needsShortDoubles;
            // w * 100 overflows to infinity for the huge ones, and infinity is not JSON. (A whole number between
            // 2^31 and 1e17 is written without a point and JsonBox reads it back as an int that saturates: not the
            // map's doing, so the values here stay out of that range.)
            auto som = mapWith(4u, 1u, 1u, {-3.0990927021096673e+307, 1.7e308, 1.5e17, 0.123456}, false);
            som.setWeightDecimals(2);
            auto text = jsonText(som);
            EXPECT_EQ(std::string::npos, text.find("inf"));
            EXPECT_EQ(std::string::npos, text.find("nan"));
            auto w = allWeights(fromText(text));
            ASSERT_EQ(4u, w.size());
            EXPECT_EQ(-3.0990927021096673e+307, w[0]);
            EXPECT_EQ(1.7e308, w[1]);
            EXPECT_EQ(1.5e17, w[2]);
            EXPECT_EQ(0.12, w[3]); // and the ordinary one is still rounded
        }
        TEST(SelfOrganizingMapTest, WeightDecimalsCanBeChangedClearedAndAreValidated){
            SelfOrganizingMap<> som(2u, 2u, 2u);
            som.setWeightDecimals(3);
            EXPECT_EQ(3, *som.getWeightDecimals());
            auto copy = som;
            EXPECT_EQ(3, *copy.getWeightDecimals());
            som.setWeightDecimals(std::nullopt);
            EXPECT_FALSE(som.getWeightDecimals().has_value());
            EXPECT_THROW(som.setWeightDecimals(-1), std::invalid_argument);
            EXPECT_THROW(som.setWeightDecimals(18), std::invalid_argument);
            EXPECT_NO_THROW(som.setWeightDecimals(0));
            EXPECT_NO_THROW(som.setWeightDecimals(17));
        }
        TEST(SelfOrganizingMapTest, TrainSkipsInvalidSamplesAndReportsTheirIndices){
            SelfOrganizingMap<> som(3u, 3u, 2u);
            std::vector<std::vector<double>> samples = {
                {0.1, 0.1},
                {0.2, 0.9}, // wrong size below
                {0.9, 0.1, 0.5},
                {0.5, std::numeric_limits<double>::quiet_NaN()}, // NaN
                {0.8, 0.8}
            };
            auto report = som.train(samples, 5u, 0.5, 2.0,
                                    Scheduler<ConstantLR>(ConstantLR{}),
                                    Scheduler<ConstantLR>(ConstantLR{}));
            EXPECT_EQ((std::vector<std::size_t>{2u, 3u}), report.skipped); // the wrong-size one and the NaN one
            EXPECT_EQ(5u, report.epochs.size());
        }
        TEST(SelfOrganizingMapTest, TrainReportsTheScheduleAndTheDistanceBeforeEachUpdate){
            // one node, one sample at 1, the weight starts at 0 and moves half way every epoch
            auto som = mapWith(1u, 1u, 1u, {0.0}, false);
            auto report = som.train({{1.0}}, 3u, 0.5, 2.0, Scheduler<ConstantLR>(ConstantLR{}), Scheduler<ConstantLR>(ConstantLR{}));
            ASSERT_EQ(3u, report.epochs.size());
            EXPECT_DOUBLE_EQ(1.0, report.epochs[0].meanBmuDistance);
            EXPECT_DOUBLE_EQ(0.5, report.epochs[1].meanBmuDistance);
            EXPECT_DOUBLE_EQ(0.25, report.epochs[2].meanBmuDistance);
            for(std::size_t i=0;i<3u;++i){
                EXPECT_EQ(i, report.epochs[i].epoch);
                EXPECT_DOUBLE_EQ(0.5, report.epochs[i].learningRate);
                EXPECT_DOUBLE_EQ(2.0, report.epochs[i].radius);
            }
            EXPECT_DOUBLE_EQ(0.875, som.getPrototype(0u, 0u)[0]);
        }
        TEST(SelfOrganizingMapTest, TrainReportsTheWinsOfTheLastEpochAndNothingWithoutUsableSamples){
            auto som = mapWith(2u, 1u, 1u, {0.0, 10.0}, false);
            auto report = som.train({{0.1}, {0.2}, {9.0}}, 2u, 0.0, 1.0, Scheduler<ConstantLR>(ConstantLR{}), Scheduler<ConstantLR>(ConstantLR{}));
            EXPECT_EQ((std::vector<std::size_t>{2u, 1u}), report.nodeCounts);
            auto none = som.train({{1.0, 2.0}}, 5u, 0.5, 1.0, Scheduler<ConstantLR>(ConstantLR{}), Scheduler<ConstantLR>(ConstantLR{}));
            EXPECT_TRUE(none.epochs.empty());
            EXPECT_EQ((std::vector<std::size_t>{0u}), none.skipped);
            EXPECT_EQ((std::vector<std::size_t>{0u, 0u}), none.nodeCounts);
        }
        TEST(SelfOrganizingMapTest, TrainStopsWhenTheCallbackReturnsFalse){
            randomGen().setSeed(5u);
            SelfOrganizingMap<> som(3u, 3u, 2u);
            std::vector<std::size_t> seen;
            TrainingOptions options;
            options.onEpoch = [&seen](const EpochInfo& info){
                seen.push_back(info.epoch);
                return info.epoch < 2u;
            };
            auto report = som.train(randomSamples(10u, 2u), 50u, 0.5, 1.5, Scheduler<ExponentialLR>(ExponentialLR{0.9}),
                                    Scheduler<ExponentialLR>(ExponentialLR{0.9}), options);
            EXPECT_EQ((std::vector<std::size_t>{0u, 1u, 2u}), seen);
            EXPECT_EQ(3u, report.epochs.size()); // the epoch that asked to stop is part of the report
        }
        TEST(SelfOrganizingMapTest, TrainInPiecesFollowsOneScheduleForAnyScheduler){
            auto samples = randomSamples(12u, 3u);
            TrainingOptions rest;
            rest.epochOffset = 4u;
            SelfOrganizingMap<> a(3u, 3u, 3u);
            SelfOrganizingMap<> b(3u, 3u, 3u);
            auto whole = a.train(samples, 7u, 0.8, 2.0, Scheduler<StepLR>(StepLR{3u, 0.5}), Scheduler<LinearLR>(LinearLR{2u, 0.3, 0.0}));
            auto first = b.train(samples, 4u, 0.8, 2.0, Scheduler<StepLR>(StepLR{3u, 0.5}), Scheduler<LinearLR>(LinearLR{2u, 0.3, 0.0}));
            auto second = b.train(samples, 3u, 0.8, 2.0, Scheduler<StepLR>(StepLR{3u, 0.5}), Scheduler<LinearLR>(LinearLR{2u, 0.3, 0.0}), rest);
            ASSERT_EQ(7u, whole.epochs.size());
            ASSERT_EQ(4u, first.epochs.size());
            ASSERT_EQ(3u, second.epochs.size());
            first.epochs.insert(first.epochs.end(), second.epochs.begin(), second.epochs.end());
            for(std::size_t i=0;i<7u;++i){
                EXPECT_EQ(i, first.epochs[i].epoch);
                EXPECT_DOUBLE_EQ(whole.epochs[i].learningRate, first.epochs[i].learningRate);
                EXPECT_DOUBLE_EQ(whole.epochs[i].radius, first.epochs[i].radius);
            }
            EXPECT_NE(whole.epochs[0].learningRate, whole.epochs[6].learningRate); // the schedule did something
        }
        namespace{
            template<class Algo, class Make>
            void expectOneScheduleInPiecesAndInOneCall(Make make, const std::vector<double>& expectedLearningRates){
                auto samples = randomSamples(12u, 3u);
                auto run = [&](bool inPieces, std::vector<double>& rates){
                    randomGen().setSeed(9u);
                    SelfOrganizingMap<> som(3u, 3u, 3u);
                    TrainingOptions options;
                    options.seed = 3u;
                    auto collect = [&rates](const TrainingReport& report){
                        for(const auto& e : report.epochs){
                            rates.push_back(e.learningRate);
                        }
                    };
                    if(inPieces){
                        collect(som.train(samples, 4u, 0.8, 3.0, Scheduler<Algo>(make()), Scheduler<ConstantLR>(ConstantLR{}), options));
                        options.epochOffset = 4u;
                        collect(som.train(samples, 6u, 0.8, 3.0, Scheduler<Algo>(make()), Scheduler<ConstantLR>(ConstantLR{}), options));
                    }else{
                        collect(som.train(samples, 10u, 0.8, 3.0, Scheduler<Algo>(make()), Scheduler<ConstantLR>(ConstantLR{}), options));
                    }
                    return allWeights(som);
                };
                std::vector<double> whole, pieces;
                auto weightsWhole = run(false, whole);
                auto weightsPieces = run(true, pieces);
                ASSERT_EQ(expectedLearningRates.size(), whole.size());
                for(std::size_t i=0;i<whole.size();++i){
                    EXPECT_NEAR(expectedLearningRates[i], whole[i], 1e-12) << "epoch " << i;
                }
                EXPECT_EQ(whole, pieces);
                EXPECT_EQ(weightsWhole, weightsPieces);
            }
        }
        TEST(SelfOrganizingMapTest, EverySchedulerOfTheLibraryFollowsOneScheduleInPiecesAndInOneCall){
            expectOneScheduleInPiecesAndInOneCall<ConstantLR>([]{ return ConstantLR{}; },
                {0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8});
            expectOneScheduleInPiecesAndInOneCall<ExponentialLR>([]{ return ExponentialLR{0.5}; },
                {0.4, 0.2, 0.1, 0.05, 0.025, 0.0125, 0.00625, 0.003125, 0.0015625, 0.00078125});
            expectOneScheduleInPiecesAndInOneCall<MultiplicativeLR>([]{ return MultiplicativeLR{0.5}; },
                {0.4, 0.2, 0.1, 0.05, 0.025, 0.0125, 0.00625, 0.003125, 0.0015625, 0.00078125});
            // acts on epoch 0, 3, 6 and 9: the first epoch already runs decayed
            expectOneScheduleInPiecesAndInOneCall<StepLR>([]{ return StepLR{3u, 0.5}; },
                {0.4, 0.4, 0.4, 0.2, 0.2, 0.2, 0.1, 0.1, 0.1, 0.05});
            expectOneScheduleInPiecesAndInOneCall<LinearLR>([]{ return LinearLR{2u, 0.1, 0.05}; },
                {0.7, 0.7, 0.6, 0.6, 0.5, 0.5, 0.4, 0.4, 0.3, 0.3});
            // acts on the epochs 3 and 6 only, and a fresh one is replayed from epoch 0 in the second call
            expectOneScheduleInPiecesAndInOneCall<MultiStepLR>([]{ return MultiStepLR{std::vector<std::size_t>{3u, 6u}, 0.5}; },
                {0.8, 0.8, 0.8, 0.4, 0.4, 0.4, 0.2, 0.2, 0.2, 0.2});
        }
        TEST(SelfOrganizingMapTest, TrainWithASeedGivesTheSameWeightsInOnePieceOrMany){
            auto samples = randomSamples(15u, 4u);
            auto weights = [&samples](std::uint64_t seed, bool inPieces){
                randomGen().setSeed(9u); // same starting weights
                SelfOrganizingMap<> som(4u, 3u, 4u);
                TrainingOptions options;
                options.seed = seed;
                if(inPieces){
                    som.train(samples, 3u, 0.6, 2.0, Scheduler<ExponentialLR>(ExponentialLR{0.9}), Scheduler<ExponentialLR>(ExponentialLR{0.9}), options);
                    options.epochOffset = 3u;
                    som.train(samples, 5u, 0.6, 2.0, Scheduler<ExponentialLR>(ExponentialLR{0.9}), Scheduler<ExponentialLR>(ExponentialLR{0.9}), options);
                }else{
                    som.train(samples, 8u, 0.6, 2.0, Scheduler<ExponentialLR>(ExponentialLR{0.9}), Scheduler<ExponentialLR>(ExponentialLR{0.9}), options);
                }
                return allWeights(som);
            };
            EXPECT_EQ(weights(1u, false), weights(1u, true));
            EXPECT_EQ(weights(1u, false), weights(1u, false));
            EXPECT_NE(weights(1u, false), weights(2u, false));
        }
        TEST(SelfOrganizingMapTest, TrainFlushesWeightsThatDecayTowardsZeroInsteadOfLeavingDenormals){
            // the weight starts at 1 and the sample is 0: it halves every epoch, and would pass through the
            // denormals (about 1e-308 to 5e-324) around epoch 1023 to 1075
            auto som = mapWith(1u, 1u, 1u, {1.0}, false);
            bool denormalSeen = false;
            bool reachedZero = false;
            TrainingOptions options;
            options.onEpoch = [&](const EpochInfo&){
                auto w = som.getPrototype(0u, 0u)[0];
                denormalSeen = denormalSeen || std::fpclassify(w) == FP_SUBNORMAL;
                reachedZero = reachedZero || w == 0.0;
                return true;
            };
            som.train({{0.0}}, 1200u, 0.5, 1.0, Scheduler<ConstantLR>(ConstantLR{}), Scheduler<ConstantLR>(ConstantLR{}), options);
            EXPECT_FALSE(denormalSeen);
            EXPECT_TRUE(reachedZero);
            EXPECT_EQ(0.0, som.getPrototype(0u, 0u)[0]);
        }
        TEST(SelfOrganizingMapTest, TrainKeepsSmallButMeaningfulWeights){
            auto som = mapWith(1u, 1u, 2u, {1e-90, 1.0}, false);
            som.train({{1e-90, 1.0}}, 3u, 0.5, 1.0, Scheduler<ConstantLR>(ConstantLR{}), Scheduler<ConstantLR>(ConstantLR{}));
            EXPECT_EQ(1e-90, som.getPrototype(0u, 0u)[0]);
        }
        TEST(SelfOrganizingMapTest, TrainDoesNotUpdateNodesThatBarelyFeelTheSample){
            // 40 nodes in a row, all 0; the sample at 1 is won by node 0, node 30 is thirty cells away
            auto som = mapWith(40u, 1u, 1u, std::vector<double>(40u, 0.0), false);
            som.train({{1.0}}, 1u, 0.5, 1.0, Scheduler<ConstantLR>(ConstantLR{}), Scheduler<ConstantLR>(ConstantLR{}));
            EXPECT_GT(som.getPrototype(0u, 0u)[0], 0.0);
            EXPECT_GT(som.getPrototype(1u, 0u)[0], 0.0);
            EXPECT_EQ(0.0, som.getPrototype(30u, 0u)[0]); // an influence of about 1e-196 would have made it a denormal
            EXPECT_EQ(0.0, som.getPrototype(39u, 0u)[0]);
        }
        TEST(SelfOrganizingMapTest, TrainAndClassifySeparatesWellSeparatedClusters){
            randomGen().setSeed(42u); // deterministic: init + per-epoch shuffles
            SelfOrganizingMap<> som(5u, 5u, 2u);
            std::vector<std::vector<double>> samples;
            // two well-separated clusters in a toy 2D feature space
            for(auto i=0u;i<20u;++i){
                samples.push_back({randomGen().random(0.0, 0.1), randomGen().random(0.0, 0.1)});
            }
            for(auto i=0u;i<20u;++i){
                samples.push_back({randomGen().random(0.9, 1.0), randomGen().random(0.9, 1.0)});
            }
            auto report = som.train(samples, 100u, 0.5, 2.5,
                                    Scheduler<ExponentialLR>(ExponentialLR{0.05}),
                                    Scheduler<ExponentialLR>(ExponentialLR{0.05}));
            EXPECT_TRUE(report.skipped.empty());
            auto bmuA = som.classify({0.05, 0.05});
            auto bmuB = som.classify({0.95, 0.95});
            ASSERT_TRUE(bmuA.has_value());
            ASSERT_TRUE(bmuB.has_value());
            EXPECT_FALSE(bmuA->x == bmuB->x && bmuA->y == bmuB->y);
            // same cluster, different point -> should land on the same or an
            // adjacent node, not clear across the grid.
            auto bmuA2 = som.classify({0.08, 0.02});
            ASSERT_TRUE(bmuA2.has_value());
            auto gx = static_cast<double>(bmuA->x) - static_cast<double>(bmuA2->x);
            auto gy = static_cast<double>(bmuA->y) - static_cast<double>(bmuA2->y);
            EXPECT_LE(std::sqrt(gx*gx + gy*gy), 2.0);
        }
        TEST(SelfOrganizingMapTest, SchedulersAreChainedAcrossEpochsNotResetFromBase){
            g_lrLog.clear();
            g_radiusLog.clear();
            SelfOrganizingMap<> som(3u, 3u, 2u);
            std::vector<std::vector<double>> samples = {{0.1,0.1},{0.5,0.5},{0.9,0.9}};
            som.train(samples, 5u, 100.0, 50.0,
                      Scheduler<LoggingLR>(LoggingLR{}),
                      Scheduler<LoggingRadius>(LoggingRadius{}));
            // each epoch must be called with the PREVIOUS epoch's result,
            std::vector<double> expectedLR = {100.0, 99.0, 98.0, 97.0, 96.0};
            std::vector<double> expectedRadius = {50.0, 49.0, 48.0, 47.0, 46.0};
            EXPECT_EQ(expectedLR, g_lrLog);
            EXPECT_EQ(expectedRadius, g_radiusLog);
        }
        TEST(SelfOrganizingMapTest, ToJsonRoundTrip){
            randomGen().setSeed(7u);
            SelfOrganizingMap<> som(3u, 4u, 4u);
            std::vector<std::vector<double>> samples;
            for(auto i=0u;i<10u;++i){
                samples.push_back({randomGen().random(0.0,1.0), randomGen().random(0.0,1.0),
                                    randomGen().random(0.0,1.0), randomGen().random(0.0,1.0)});
            }
            som.train(samples, 10u, 0.5, 1.5,
                      Scheduler<ConstantLR>(ConstantLR{}),
                      Scheduler<ConstantLR>(ConstantLR{}));
            auto somJson = som.toJson();
            SelfOrganizingMap<> som2(somJson.getObject());
            EXPECT_EQ(som.getWidth(), som2.getWidth());
            EXPECT_EQ(som.getHeight(), som2.getHeight());
            EXPECT_EQ(som.getInputDim(), som2.getInputDim());
            for(std::size_t y=0;y<som.getHeight();++y){
                for(std::size_t x=0;x<som.getWidth();++x){
                    EXPECT_EQ(som.getPrototype(x,y), som2.getPrototype(x,y));
                }
            }
            EXPECT_EQ(somJson, som2.toJson());
        }

        namespace{
            JsonBox::Object somObject(const std::string& width, const std::string& height, const std::string& inputDim, std::size_t weights){
                JsonBox::Object o;
                o["width"] = JsonBox::Value(width);
                o["height"] = JsonBox::Value(height);
                o["inputDim"] = JsonBox::Value(inputDim);
                o["toroidal"] = JsonBox::Value(true);
                JsonBox::Array w;
                for(std::size_t i = 0; i < weights; ++i){
                    w.push_back(JsonBox::Value(0.5));
                }
                o["weights"] = JsonBox::Value(w);
                return o;
            }
            std::size_t halfRange(){
                return std::size_t{1} << (std::numeric_limits<std::size_t>::digits / 2);
            }
        }
        TEST(SelfOrganizingMapTest, JsonCtorRejectsDimensionsWhoseProductOverflows){
            // the product wraps to 0, which an empty weights array would otherwise match
            EXPECT_THROW(SelfOrganizingMap<>(somObject(std::to_string(halfRange()), std::to_string(halfRange()), "1", 0u)), std::invalid_argument);
        }
        TEST(SelfOrganizingMapTest, JsonCtorRejectsHugeInputDimWithoutAllocatingForIt){
            // must fail on the weights size, not by trying to allocate inputDim doubles
            EXPECT_THROW(SelfOrganizingMap<>(somObject("2", "2", "1000000000000000000", 4u)), std::invalid_argument);
        }
        TEST(SelfOrganizingMapTest, SizedCtorRejectsDimensionsWhoseProductOverflows){
            EXPECT_THROW(SelfOrganizingMap<>(halfRange(), halfRange(), 1u), std::invalid_argument);
        }
    }
}
#endif // EVOAI_SELF_ORGANIZING_MAP_TEST_HPP
