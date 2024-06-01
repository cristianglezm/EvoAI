#ifndef EVOAI_SCHEDULERS_TEST_HPP
#define EVOAI_SCHEDULERS_TEST_HPP

#include <limits>

#include <gtest/gtest.h>
#include <EvoAI.hpp>

namespace EvoAI{
    namespace Test{
        TEST(SchedulersAlgoTest, ConstantLR){
            double lr = 100.0;
            auto cLr = ConstantLR();
            EXPECT_EQ(lr, cLr(lr, 0u));
            EXPECT_EQ(lr, cLr(lr, 1u));
            auto cLrJson = cLr.toJson();
            auto cLr2 = ConstantLR(cLrJson.getObject());
            EXPECT_EQ(cLrJson, cLr2.toJson());
        }
        TEST(SchedulersAlgoTest, ExponentialLR){
            double lr = 100.0;
            auto eLr = ExponentialLR(0.1);
            EXPECT_EQ(10.0, eLr(lr, 0u));
            auto eLrJson = eLr.toJson();
            auto eLr2 = ExponentialLR(eLrJson.getObject());
            EXPECT_EQ(eLrJson, eLr2.toJson());
        }
        TEST(SchedulersAlgoTest, MultiplicativeLR){
            double lr = 100.0;
            auto mlLr = MultiplicativeLR(0.1);
            EXPECT_EQ(10.0, mlLr(lr, 0u));
            auto mlLrJson = mlLr.toJson();
            auto mlLr2 = MultiplicativeLR(mlLrJson.getObject());
            EXPECT_EQ(mlLrJson, mlLr2.toJson());
        }
        TEST(SchedulersAlgoTest, MultiStepLR){
            double lr = 100.0;
            auto msLr = MultiStepLR({1u, 5u, 9u}, 0.1); // lr changes at 1, 9, 5 epochs
            lr = msLr(lr, 0u);
            EXPECT_EQ(100.0, lr);// should not change
            lr = msLr(lr, 1u);
            EXPECT_EQ(10.0, lr);
            lr = msLr(lr, 5u);
            EXPECT_EQ(1.0, lr);
            lr = msLr(lr, 9u);
            EXPECT_EQ(0.1, lr);
            lr = msLr(lr, 10u);
            EXPECT_EQ(0.1, lr); // should not change
            auto msLrJson = msLr.toJson();
            auto msLr2 = MultiStepLR(msLrJson.getObject());
            EXPECT_EQ(msLrJson, msLr2.toJson());
        }
        TEST(SchedulersAlgoTest, StepLR){
            double lr = 100.0;
            auto sLr = StepLR(10, 0.1); // lr changes every 10 epochs
            lr = sLr(lr, 0u);
            EXPECT_EQ(10.0, lr);
            lr = sLr(lr, 5u);
            EXPECT_EQ(10.0, lr); // should not change
            lr = sLr(lr, 10u);
            EXPECT_EQ(1.0, lr);
            lr = sLr(lr, 20u);
            EXPECT_EQ(0.1, lr);
            auto sLrJson = sLr.toJson();
            auto sLr2 = StepLR(sLrJson.getObject());
            EXPECT_EQ(sLrJson, sLr2.toJson());
        }
        TEST(SchedulerTest, SchedulerTest){
            Scheduler<MultiStepLR> sch(MultiStepLR{{1u,9u,5u}, 0.1});
            auto lr = sch.step(100.0, 0u);
            EXPECT_EQ(100.0, lr);
            lr = sch.step(lr, 1u);
            EXPECT_EQ(10.0, lr);
            auto schJson = sch.toJson();
            auto sch2 = Scheduler<MultiStepLR>(schJson.getObject());
            EXPECT_EQ(schJson, sch2.toJson());
            //struct Dummy{};
            //Scheduler<Dummy> schdummy; // compilation error as intended(dummy does not fulfill meta::is_a_scheduler_algorithm_v<Dummy> requirements)
        }
    }
}
#endif // EVOAI_SCHEDULERS_TEST_HPP
