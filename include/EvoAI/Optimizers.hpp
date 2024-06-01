#ifndef EVOAI_OPTIMIZERS_HPP
#define EVOAI_OPTIMIZERS_HPP

#include <EvoAI/Optimizers/SGD.hpp>
#include <EvoAI/Optimizers/Adam.hpp>

#include <EvoAI/Schedulers.hpp>
#include <JsonBox.h>

namespace EvoAI{
    class NeuralNetwork;
    /**
     * @brief Optimizer
     * @tparam Algo is an Optimizer algorithm
     * @tparam SchedulerAlgo is an Scheduler algorithm
     * @details
     *   Algo needs to fulfill these conditions: <br />
     *      Algo has a constructor Algo::Algo(JsonBox::Object, std::vector<Connection*>) <br />
     *      Algo has a member function JsonBox::Value toJson() const noexcept <br />
     *      Algo has a double operator()(double, std::size_t) const noexcept <br />
     */
    template<class Algo, class SchedulerAlgo>
    class Optimizer final{
        public:
            static_assert(meta::is_an_optimizer_algorithm_v<Algo, std::vector<Connection*>>, "Algo requires to fulfill EvoAI::is_an_optimizer_algorithm_v, more info at Utils/TypeUtils.hpp");
            using AlgoType = Algo;
            using SchedulerType = Scheduler<SchedulerAlgo>;
            /**
             * @brief constructor
             * @param lr double learning rate
             * @param batchSize std::size_t batch size
             * @param algo Algo to use (SGD, Adam, ...)
             * @param scheduler Scheduler<SchedulerAlgo> to use (ConstantLR, Exponential, MultiStepLR, ...)
             */
            Optimizer(double lr, std::size_t batchSize, const Algo& algo, const Scheduler<SchedulerAlgo>& scheduler);
            /**
             * @brief constructor for JsonBox::Object
             * @param o JsonBox::Object
             * @param parameters std::vector<Connection*>&&
             */
            Optimizer(JsonBox::Object o, std::vector<Connection*>&& parameters);
            /**
             * @brief converts to a JsonBox::Value
             * @return JsonBox::Value
             */
            JsonBox::Value toJson() const noexcept;
            /**
             * @brief setter for batch size
             */
            void setBatchSize(std::size_t batchsize) noexcept;
            /**
             * @brief getter for batch size
             * @return std::size_t 
             */
            std::size_t getBatchSize() const noexcept;
            /**
             * @brief step to update weights and lr if applies.
             * @param epoch
             */
            void step(std::size_t epoch) noexcept;
            /**
             * @brief reset gradients
             */
            void zeroGrad() noexcept;
            /**
             * @brief get Algo(SGD, Adam, ...)
             * @return Algo&
             */
            Algo& getAlgo() noexcept;
            /**
             * @brief get Scheduler
             * @return Scheduler<SchedulerAlgo>&
             */
            Scheduler<SchedulerAlgo>& getScheduler() noexcept;
            ~Optimizer() = default;
        private:
            Algo m_stepFn;
            Scheduler<SchedulerAlgo> m_scheduler;
            std::size_t m_batchSize;
            double m_lr;
    };
// Optimizer Impl
    template<class Algo, class SchedulerAlgo>
    Optimizer<Algo, SchedulerAlgo>::Optimizer(double lr, std::size_t batchSize, const Algo& algo, const Scheduler<SchedulerAlgo>& scheduler)
    : m_stepFn(algo)
    , m_scheduler(scheduler)
    , m_batchSize(batchSize)
    , m_lr(lr){}
    template<class Algo, class SchedulerAlgo>
    Optimizer<Algo, SchedulerAlgo>::Optimizer(JsonBox::Object o, std::vector<Connection*>&& parameters)
    : m_stepFn(o["stepFn"].getObject(), std::forward<std::vector<Connection*>>(parameters))
    , m_scheduler(o["scheduler"].getObject())
    , m_batchSize(std::stoul(o["batchSize"].tryGetString("1")))
    , m_lr(o["lr"].tryGetDouble(0.1)){}
    template<class Algo, class SchedulerAlgo>
    JsonBox::Value Optimizer<Algo, SchedulerAlgo>::toJson() const noexcept{
        JsonBox::Object o;
        o["stepFn"] = m_stepFn.toJson();
        o["scheduler"] = m_scheduler.toJson();
        o["batchSize"] = std::to_string(m_batchSize);
        o["lr"] = m_lr;
        return o;
    }
    template<class Algo, class SchedulerAlgo>
    void Optimizer<Algo, SchedulerAlgo>::setBatchSize(std::size_t batchSize) noexcept{
        m_batchSize = batchSize;
    }
    template<class Algo, class SchedulerAlgo>
    std::size_t Optimizer<Algo, SchedulerAlgo>::getBatchSize() const noexcept{
        return m_batchSize;
    }
    template<class Algo, class SchedulerAlgo>
    void Optimizer<Algo, SchedulerAlgo>::step(std::size_t epoch) noexcept{
        m_stepFn(m_lr, m_batchSize);
        m_lr = m_scheduler.step(m_lr, epoch);
    }
    template<class Algo, class SchedulerAlgo>
    void Optimizer<Algo, SchedulerAlgo>::zeroGrad() noexcept{
        m_stepFn.zeroGrad();
    }
    template<class Algo, class SchedulerAlgo>
    Algo& Optimizer<Algo, SchedulerAlgo>::getAlgo() noexcept{
        return m_stepFn;
    }
    template<class Algo, class SchedulerAlgo>
    Scheduler<SchedulerAlgo>& Optimizer<Algo, SchedulerAlgo>::getScheduler() noexcept{
        return m_scheduler;
    }
}

#endif // EVOAI_OPTIMIZERS_HPP