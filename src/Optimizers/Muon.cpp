#include <EvoAI/Optimizers/Muon.hpp>
#include <EvoAI/Connection.hpp>

#include <cmath>
#include <utility>

namespace EvoAI{
    Muon::Muon(std::vector<Connection*>&& parameters, bool maximize, double mu, double alpha, double epsilon, bool accumGradients)
    : m_params(std::forward<std::vector<Connection*>>(parameters))
    , m_mu(mu)
    , m_alpha(alpha)
    , m_velocity()
    , m_rms()
    , m_epsilon(epsilon)
    , m_maximize(maximize)
    , m_accumGradients(accumGradients){
        auto n = m_params.size();
        m_velocity.assign(n, 0.0);
        m_rms.assign(n, 0.0);
    }

    Muon::Muon(JsonBox::Object o, std::vector<Connection*>&& parameters)
    : m_params(std::forward<std::vector<Connection*>>(parameters))
    , m_mu(o["mu"].tryGetDouble(0.9))
    , m_alpha(o["alpha"].tryGetDouble(0.99))
    , m_velocity()
    , m_rms()
    , m_epsilon(o["epsilon"].tryGetDouble(1e-8))
    , m_maximize(o["maximize"].tryGetBoolean(false))
    , m_accumGradients(o["accumGradients"].tryGetBoolean(false)){
        auto parseJsonArray = [&o](const std::string& name, std::vector<double>& into){
            auto& arr = o[name].getArray();
            into.reserve(arr.size());
            for(const auto& d:arr){
                into.emplace_back(d.getDouble());
            }
        };
        parseJsonArray("velocity", m_velocity);
        parseJsonArray("rms", m_rms);
        // If arrays are missing or empty, ensure vectors have the correct size
        auto n = m_params.size();
        if(m_velocity.size() != n) m_velocity.assign(n, 0.0);
        if(m_rms.size() != n) m_rms.assign(n, 0.0);
    }

    JsonBox::Value Muon::toJson() const noexcept{
        JsonBox::Object o;
        o["name"] = "Muon";
        o["mu"] = m_mu;
        o["alpha"] = m_alpha;
        auto toJsonArray = [](const std::vector<double>& from) -> JsonBox::Array{
            JsonBox::Array arr;
            arr.reserve(from.size());
            for(const auto& f:from){
                arr.emplace_back(f);
            }
            return arr;
        };
        o["velocity"] = toJsonArray(m_velocity);
        o["rms"] = toJsonArray(m_rms);
        o["epsilon"] = m_epsilon;
        o["maximize"] = m_maximize;
        o["accumGradients"] = m_accumGradients;
        return o;
    }

    void Muon::operator()(double lr, std::size_t batchSize) noexcept{
        auto paramsSize = m_params.size();
        for(auto i=0u;i<paramsSize;++i){
            auto c = m_params[i];
            if(!c->isRecurrent()){ // keep context neurons with weight 1.0
                if(!m_accumGradients){
                    c->setGradient(c->getGradient() / batchSize);
                }
                auto grad = c->getGradient();
                // update rms accumulator (RMSProp-style)
                m_rms[i] = m_alpha * m_rms[i] + (1.0 - m_alpha) * (grad * grad);
                // compute scaled gradient
                auto scaled = grad / (std::sqrt(m_rms[i]) + m_epsilon);
                // update velocity (momentum)
                m_velocity[i] = m_mu * m_velocity[i] + lr * scaled;
                // apply update (sign depends on maximize)
                if(m_maximize){
                    c->setWeight(c->getWeight() + m_velocity[i]);
                }else{
                    c->setWeight(c->getWeight() - m_velocity[i]);
                }
            }
        }
    }

    void Muon::zeroGrad() noexcept{
        for(auto c:m_params){
            c->reset();
        }
    }
}
