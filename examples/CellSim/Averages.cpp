#include "Averages.hpp"

namespace EvoAI{
        Averages::Averages() noexcept
        : Avgs(8){}
        Averages::Averages(JsonBox::Object o) noexcept
        : Avgs(8){
            std::array<std::string, 8> names = {"AtkPowerAvg","DefenseAvg","HealthAvg","NutrientsAvg",
                                                "StaminaAvg", "HungerAvg", "avgFitness", "numSpecies"};
            for(auto i=0u;i<Avgs.size();++i){
                for(auto j=0u;j<o[names[i]].getArray().size();++j){
                    Avgs[i].emplace_back(o[names[i]].getArray()[j].tryGetDouble(0.0));
                }
            }
        }
        JsonBox::Value Averages::toJson() const noexcept{
            JsonBox::Value v;
            std::array<std::string, 8> names = {"AtkPowerAvg","DefenseAvg","HealthAvg","NutrientsAvg",
                                                "StaminaAvg", "HungerAvg", "avgFitness", "numSpecies"};
            for(auto i=0u;i<Avgs.size();++i){
                JsonBox::Array arr;
                arr.reserve(Avgs[i].size());
                for(auto j=0u;j<Avgs[i].size();++j){
                    arr.emplace_back(Avgs[i][j]);
                }
                v[names[i]] = arr;
                arr.clear();
            }
            return v;
        }
        void Averages::calcAvgs(Population<Cell*>& pop) noexcept{
            auto& m = pop.getMembers();
            std::array<double, 6> sums;
            for(auto i=0u;i<m.size();++i){
                auto stats = m[i]->getMaxStats();
                sums[0] += stats.atkPower; 
                sums[1] += stats.defense;
                sums[2] += stats.health;
                sums[3] += stats.nutrients;
                sums[4] += stats.stamina;
                sums[5] += stats.hunger;
            }
            for(auto i=0u;i<sums.size();++i){
                Avgs[i].emplace_back(sums[i] / pop.getPopulationMaxSize());
            }
            Avgs[6].emplace_back(pop.computeAvgFitness());
            Avgs[7].emplace_back(pop.getSpeciesSize());
        }
        void Averages::writeToFile(std::string_view filename) noexcept{
            writeMultiPlot(filename, {"AtkPowerAvg","DefenseAvg","HealthAvg","NutrientsAvg","StaminaAvg", "HungerAvg", "avgFitness", "numSpecies"},
            Avgs);
        }
}