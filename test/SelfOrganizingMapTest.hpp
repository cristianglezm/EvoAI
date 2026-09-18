#ifndef EVOAI_SELF_ORGANIZING_MAP_TEST_HPP
#define EVOAI_SELF_ORGANIZING_MAP_TEST_HPP

#include <limits>

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
            EXPECT_DOUBLE_EQ(0.0, dist({1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}));
            EXPECT_DOUBLE_EQ(5.0, dist({0.0, 0.0}, {3.0, 4.0}));
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
        TEST(SelfOrganizingMapTest, TrainSkipsInvalidSamplesAndReturnsCount){
            SelfOrganizingMap<> som(3u, 3u, 2u);
            std::vector<std::vector<double>> samples = {
                {0.1, 0.1},
                {0.2, 0.9}, // wrong size below
                {0.9, 0.1, 0.5},
                {0.5, std::numeric_limits<double>::quiet_NaN()}, // NaN
                {0.8, 0.8}
            };
            auto skipped = som.train(samples, 5u, 0.5, 2.0,
                                      Scheduler<ConstantLR>(ConstantLR{}),
                                      Scheduler<ConstantLR>(ConstantLR{}));
            EXPECT_EQ(2u, skipped); // the wrong-size one and the NaN one
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
            auto skipped = som.train(samples, 100u, 0.5, 2.5,
                                      Scheduler<ExponentialLR>(ExponentialLR{0.05}),
                                      Scheduler<ExponentialLR>(ExponentialLR{0.05}));
            EXPECT_EQ(0u, skipped);
            auto bmuA = som.classify({0.05, 0.05});
            auto bmuB = som.classify({0.95, 0.95});
            ASSERT_TRUE(bmuA.has_value());
            ASSERT_TRUE(bmuB.has_value());
            EXPECT_NE(bmuA.value(), bmuB.value());
            // same cluster, different point -> should land on the same or an
            // adjacent node, not clear across the grid.
            auto bmuA2 = som.classify({0.08, 0.02});
            ASSERT_TRUE(bmuA2.has_value());
            auto gx = static_cast<double>(bmuA->first) - static_cast<double>(bmuA2->first);
            auto gy = static_cast<double>(bmuA->second) - static_cast<double>(bmuA2->second);
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
    }
}
#endif // EVOAI_SELF_ORGANIZING_MAP_TEST_HPP
