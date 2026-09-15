#include <EvoAI/Evaluators/NetworkMetrics.hpp>

namespace EvoAI{
    JsonBox::Value NetworkMetrics::toJson() const noexcept{
        JsonBox::Object o;
        o["taskScore"] = taskScore;
        o["latency"] = latency;
        o["memoryBytes"] = memoryBytes;
        o["numConnections"] = numConnections;
        o["numNeurons"] = numNeurons;
        o["parameterCount"] = parameterCount;
        o["sparsity"] = sparsity;
        o["fitness"] = fitness;
        return JsonBox::Value(o);
    }
}
