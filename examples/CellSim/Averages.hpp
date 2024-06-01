#ifndef EVOAI_AVERAGES_HPP
#define EVOAI_AVERAGES_HPP

#include <array>
#include <JsonBox.h>
#include <EvoAI/Population.hpp>
#include "Cell.hpp"

namespace EvoAI{
    /**
     * @brief struct to keep the averages of Cells.
     */
    struct Averages final{
        Averages() noexcept;
        Averages(JsonBox::Object o) noexcept;
        JsonBox::Value toJson() const noexcept;
        void calcAvgs(Population<Cell*>& pop) noexcept;
        void writeToFile(std::string_view filename) noexcept;
        // data
        std::vector<std::vector<double>> Avgs;
    };
}

#endif // EVOAI_AVERAGES_HPP