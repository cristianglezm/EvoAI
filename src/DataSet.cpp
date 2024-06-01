#include <EvoAI/Utils/RandomUtils.hpp>
#include <EvoAI/DataSet.hpp>

#include <cassert>
#include <random>
#include <memory>

namespace EvoAI{
    Dataset::Dataset(std::size_t batchSize) noexcept
    : m_batchSize(batchSize)
    , m_index(0u)
    , m_data(){}
    Dataset::Dataset(Dataset::TrainingFormat&& data, std::size_t batchSize) noexcept
    : m_batchSize(batchSize)
    , m_index(0u)
    , m_data(std::forward<Dataset::TrainingFormat>(data)){}
    Dataset::Dataset(std::vector<std::vector<double>>&& inputs, std::vector<std::vector<double>>&& targets, std::size_t batchSize) noexcept
    : m_batchSize(batchSize)
    , m_index(0u)
    , m_data(){
        assert(inputs.size() == targets.size() && "inputs and targets size should match");
        m_data.reserve(inputs.size());
        for(auto i=0u;i<inputs.size();++i){
            m_data.emplace_back(std::move(inputs[i]), std::move(targets[i]));
        }
    }
    const std::pair<std::vector<double>&, std::vector<double>&> Dataset::operator()() noexcept{
        auto i = m_index;
        m_index = (m_index + 1) % m_data.size();
        return std::make_pair(std::ref(m_data[i].first), std::ref(m_data[i].second));
    }
    void Dataset::add(std::pair<std::vector<double>, std::vector<double>>&& sample) noexcept{
        m_data.emplace_back(std::forward<std::pair<std::vector<double>, std::vector<double>>>(sample));
    }
    std::size_t Dataset::size() const noexcept{
        return (m_data.size() + m_batchSize - 1) / m_batchSize;
    }
    std::size_t Dataset::getBatchSize() const noexcept{
        return m_batchSize;
    }
    void Dataset::shuffle() noexcept{
        auto g = randomGen().getEngine();
        std::uniform_int_distribution ud(0, static_cast<int>(m_data.size() - 1));
        for(auto i=0u;i<m_data.size();++i){
            auto index1 = ud(g);
            auto index2 = ud(g);
            std::swap(m_data[index1], m_data[index2]);
        }
    }
}