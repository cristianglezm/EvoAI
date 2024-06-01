#ifndef EVOAI_SCHEDULERS_HPP
#define EVOAI_SCHEDULERS_HPP

#include <JsonBox.h>

#include <EvoAI/Schedulers/ConstantLR.hpp>
#include <EvoAI/Schedulers/ExponentialLR.hpp>
#include <EvoAI/Schedulers/MultiplicativeLR.hpp>
#include <EvoAI/Schedulers/MultiStepLR.hpp>
#include <EvoAI/Schedulers/StepLR.hpp>
#include <EvoAI/Utils/TypeUtils.hpp>

namespace EvoAI{
    /**
     * @brief Scheduler to update the learning rate.
     * @tparam Algo is an scheduling algorithm
     * @details
     *   Algo needs to fulfill these conditions: <br />
     *      Algo has a member function JsonBox::Value toJson() const noexcept <br />
     *      Algo has a constructor Algo::Algo(JsonBox::Object) <br />
     *      Algo has a double operator()(double, std::size_t) const noexcept <br />
     */
    template<class Algo>
    struct Scheduler final{
        static_assert(meta::is_a_scheduler_algorithm_v<Algo>, "Algo requires to fulfill is_a_scheduler_algorithm_v, more info at Utils/TypeUtils.hpp");
        using type = Algo;
        /**
         * @brief constructor
         * @param algo Algo&& scheduling algorithm (SGD, Adam, ...)
         */
        Scheduler(Algo&& algo);
        /**
         * @brief constructor for JsonBox::Object
         * @param o JsonBox::Object
         */
        Scheduler(JsonBox::Object o);
        /**
         * @brief converts to JsonBox::Value
         * @return JsonBox::Value
         */
        JsonBox::Value toJson() const noexcept;
        /**
         * @brief applies the scheduling step
         * @param lr
         * @param epoch
         * @return double updated learning rate
         */
        double step(double lr, std::size_t epoch) const noexcept;
        /**
         * @brief returns the Algo& to change settings, etc.
         * @return Algo&
         */
        Algo& getAlgo() const noexcept;
        ~Scheduler() = default;
        // data
        Algo stepFn;
    };
    // impl
    template<typename Algo>
    Scheduler<Algo>::Scheduler(Algo&& algo)
    : stepFn(algo){}
    template<typename Algo>
    Scheduler<Algo>::Scheduler(JsonBox::Object o)
    : stepFn(o["Algo"].getObject()){}
    template<typename Algo>
    JsonBox::Value Scheduler<Algo>::toJson() const noexcept{
        JsonBox::Object o;
        o["Algo"] = stepFn.toJson();
        return o;
    }
    template<typename Algo>
    double Scheduler<Algo>::step(double lr, std::size_t epoch) const noexcept{
        return stepFn(lr, epoch);
    }
    template<typename Algo>
    Algo& Scheduler<Algo>::getAlgo() const noexcept{
        return stepFn;
    }
}
#endif // EVOAI_SCHEDULERS_HPP