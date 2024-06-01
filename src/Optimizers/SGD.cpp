#include <EvoAI/Optimizers/SGD.hpp>
#include <EvoAI/Connection.hpp>

#include <vector>

namespace EvoAI{
    SGD::SGD(std::vector<Connection*>&& parameters, double momentum, bool nesterov, bool maximize, bool accumGradients)
    : m_nesterov(nesterov)
    , m_maximize(maximize)
    , m_accumGradients(accumGradients)
    , m_params(std::forward<std::vector<Connection*>>(parameters))
    , m_momentumWeights(m_params.size(), momentum)
    , m_velocityWeights(m_params.size(), 0.0){}
    SGD::SGD(JsonBox::Object o, std::vector<Connection*>&& parameters)
    : m_nesterov(o["nesterov"].tryGetBoolean(false))
    , m_maximize(o["maximize"].tryGetBoolean(false))
    , m_accumGradients(o["accumGradients"].tryGetBoolean(false))
    , m_params(std::forward<std::vector<Connection*>>(parameters))
    , m_momentumWeights()
    , m_velocityWeights(){
        auto parseJsonArray = [&o](const std::string& name, std::vector<double>& into){
            auto& arr = o[name].getArray();
            into.reserve(arr.size());
            for(const auto& d:arr){
                into.emplace_back(d.getDouble());
            }
        };
        parseJsonArray("momentumWeights", m_momentumWeights);
        parseJsonArray("velocityWeights", m_velocityWeights);
    }
    JsonBox::Value SGD::toJson() const noexcept{
        JsonBox::Object o;
        o["name"] = "SGD";
        auto toJsonArray = [](const std::vector<double>& from) -> JsonBox::Array{
            JsonBox::Array arr;
            arr.reserve(from.size());
            for(const auto& f:from){
                arr.emplace_back(f);
            }
            return arr;
        };
        o["momentumWeights"] = toJsonArray(m_momentumWeights);
        o["velocityWeights"] = toJsonArray(m_velocityWeights);
        o["nesterov"] = m_nesterov;
        o["maximize"] = m_maximize;
        o["accumGradients"] = m_accumGradients;
        return o;
    }
    void SGD::operator()(double lr, std::size_t batchSize) noexcept{
        auto paramsSize = m_params.size();
        for(auto i=0u;i<paramsSize;++i){
            auto c = m_params[i];
            if(!c->isRecurrent()){ // needed to keep context neurons with weight 1.0
                if(!m_accumGradients){
                    c->setGradient(c->getGradient() / batchSize);
                }
                if(m_momentumWeights[0] == 0.0){
                    if(m_maximize){
                        c->setWeight(c->getWeight() + lr * c->getGradient());
                    }else{
                        c->setWeight(c->getWeight() - lr * c->getGradient());
                    }
                }else{
                    if(!m_nesterov){
                        if(m_maximize){
                            m_velocityWeights[i] = m_momentumWeights[i] * m_velocityWeights[i] + lr * c->getGradient();
                        }else{
                            m_velocityWeights[i] = m_momentumWeights[i] * m_velocityWeights[i] - lr * c->getGradient();
                        }
                        c->setWeight(c->getWeight() + m_velocityWeights[i]);
                    }else{
                        if(m_maximize){
                            m_velocityWeights[i] = m_momentumWeights[i] * m_velocityWeights[i] + lr * c->getGradient();
                            c->setWeight(c->getWeight() + m_momentumWeights[i] * m_velocityWeights[i] + lr * c->getGradient());
                        }else{
                            m_velocityWeights[i] = m_momentumWeights[i] * m_velocityWeights[i] - lr * c->getGradient();
                            c->setWeight(c->getWeight() + m_momentumWeights[i] * m_velocityWeights[i] - lr * c->getGradient());
                        }
                    }
                }
            }
        }
    }
    void SGD::zeroGrad() noexcept{
        for(auto c:m_params){
            c->reset();
        }
    }
}
