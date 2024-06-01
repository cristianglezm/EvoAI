#include <EvoAI/NeuralNetwork.hpp>

namespace EvoAI{
    Adam::Adam(std::vector<Connection*>&& parameters, bool maximize, double beta1, double beta2, double epsilon, bool accumGradients)
    : m_params(std::forward<std::vector<Connection*>>(parameters))
    , m_beta1(beta1)
    , m_beta2(beta2)
    , m_mWeight(m_params.size(), 0.0)
    , m_vWeight(m_params.size(), 0.0)
    , m_epsilon(epsilon)
    , m_t{0}
    , m_maximize(maximize)
    , m_accumGradients(accumGradients){}
    Adam::Adam(JsonBox::Object o, std::vector<Connection*>&& parameters)
    : m_params(std::forward<std::vector<Connection*>>(parameters))
    , m_beta1(o["beta1"].tryGetDouble(0.9))
    , m_beta2(o["beta2"].tryGetDouble(0.999))
    , m_mWeight()
    , m_vWeight()
    , m_epsilon(o["epsilon"].tryGetDouble(1e-8))
    , m_t{std::stoul(o["t"].tryGetString("0"))}
    , m_maximize(o["maximize"].tryGetBoolean(false))
    , m_accumGradients(o["accumGradients"].tryGetBoolean(false)){
        auto parseJsonArray = [&o](const std::string& name, std::vector<double>& into){
            auto& arr = o[name].getArray();
            into.reserve(arr.size());
            for(const auto& d:arr){
                into.emplace_back(d.getDouble());
            }
        };
        parseJsonArray("mWeight", m_mWeight);
        parseJsonArray("vWeight", m_vWeight);
    }
    JsonBox::Value Adam::toJson() const noexcept{
        JsonBox::Object o;
        o["name"] = "Adam";
        o["beta1"] = m_beta1;
        o["beta2"] = m_beta2;
        auto toJsonArray = [](const std::vector<double>& from) -> JsonBox::Array{
            JsonBox::Array arr;
            arr.reserve(from.size());
            for(const auto& f:from){
                arr.emplace_back(f);
            }
            return arr;
        };
        o["mWeight"] = toJsonArray(m_mWeight);
        o["vWeight"] = toJsonArray(m_vWeight);
        o["epsilon"] = m_epsilon;
        o["t"] = std::to_string(m_t);
        o["maximize"] = m_maximize;
        o["accumGradients"] = m_accumGradients;
        return o;
    }
    void Adam::operator()(double lr, std::size_t batchSize) noexcept{
        ++m_t;
        auto paramsSize = m_params.size();
        for(auto i=0u;i<paramsSize;++i){
            auto c = m_params[i];
            if(!c->isRecurrent()){ // needed to keep context neurons with weight 1.0
                if(!m_accumGradients){
                    c->setGradient(c->getGradient() / batchSize);
                }
                // m b1
                m_mWeight[i] = m_beta1 * m_mWeight[i] + (1.0 - m_beta1) * c->getGradient();
                // rms b2
                m_vWeight[i] = m_beta2 * m_vWeight[i] + (1.0 - m_beta2) * (c->getGradient() * c->getGradient());
                // bias correction
                auto mWcorrected = m_mWeight[i] / (1.0 - std::pow(m_beta1, m_t));
                auto vWcorrected = m_vWeight[i] / (1.0 - std::pow(m_beta2, m_t));
                // update weight
                if(m_maximize){
                    c->setWeight(c->getWeight() + lr * (mWcorrected / (std::sqrt(vWcorrected) + m_epsilon)));
                }else{
                    c->setWeight(c->getWeight() - lr * (mWcorrected / (std::sqrt(vWcorrected) + m_epsilon)));
                }
            }
        }
    }
    void Adam::zeroGrad() noexcept{
        for(auto c:m_params){
            c->reset();
        }
    }
}
