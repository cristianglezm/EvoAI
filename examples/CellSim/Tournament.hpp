#ifndef EVOAI_TOURNAMENT_HPP
#define EVOAI_TOURNAMENT_HPP

#include <EvoAI/SelectionAlgorithms.hpp>
#include "Cell.hpp"

#include <queue>

namespace EvoAI::SelectionAlgorithms{
    template<>
    Tournament<Cell*>::Tournament(std::size_t maxPopulation, std::size_t rnds) noexcept
    : maxPop(maxPopulation)
    , rounds(rnds){}
    template<>
    template<typename Members>
    std::pair<Tournament<Cell*>::pointer, Tournament<Cell*>::pointer> Tournament<Cell*>::fight(Members& members) noexcept{
        std::size_t max = members.size() - 1;
        pointer champ = nullptr;
        pointer loser = nullptr;
        if constexpr(std::is_pointer_v<Cell*> || std::is_same_v<Members, std::vector<pointer>>){
            champ = members[randomGen().random(std::size_t(0), max)];
            loser = members[randomGen().random(std::size_t(0), max)];
        }else{
            champ = &members[randomGen().random(std::size_t(0), max)];
            loser = &members[randomGen().random(std::size_t(0), max)];
        }
        for(auto i=0u;i<rounds;++i){
            pointer contender = nullptr;
            if constexpr(std::is_pointer_v<Cell*> || std::is_same_v<Members, std::vector<pointer>>){
                contender = members[randomGen().random(std::size_t(0), max)];
            }else{
                contender = &members[randomGen().random(std::size_t(0), max)];
            }
            if(contender->getFitness() > champ->getFitness()){
                loser = champ;
                champ = contender;
            }
        }
        return std::make_pair(champ,loser);
    }
    template<>
    std::vector<Selected<Cell*>> Tournament<Cell*>::operator()(std::vector<std::remove_pointer_t<Cell*>*>& members, [[maybe_unused]] std::size_t numberToSelect) noexcept{
        std::vector<Selected<Cell*>> selected;
        std::vector<pointer> livingCells;
        livingCells.reserve(members.size());
        std::queue<pointer> deadCells;
        for(auto& cell:members){
            if(cell->isAlive()){
                livingCells.emplace_back(cell);
            }else if(!cell->canBeEaten()){
                deadCells.emplace(cell);
            }
        }
        if((members.size() == livingCells.size()) || livingCells.empty()){
            return selected;
        }
        selected.reserve(deadCells.size());
        while(!deadCells.empty()){
            auto father = fight(livingCells);
            auto mother = fight(livingCells);
            if(father.first == mother.first){
                mother = fight(livingCells);
            }
            auto cell = deadCells.front();
            deadCells.pop();
            selected.emplace_back(father.first, mother.first, cell);
        }
        return selected;
    }
    template<>
    std::vector<Selected<Cell*>> Tournament<Cell*>::operator()(std::map<std::size_t, std::unique_ptr<Species<Cell*>>>& species, std::size_t numberToSelect) noexcept{
        std::vector<Selected<Cell*>> selected;
        selected.reserve(numberToSelect);
        for(auto& [id, sp]: species){
            auto& members = sp->getMembers();
            std::vector<pointer> livingCells;
            livingCells.reserve(members.size());
            std::queue<pointer> deadCells;
            for(auto& cell:members){
                if(cell->isAlive()){
                    livingCells.emplace_back(cell);
                }else if(!cell->canBeEaten()){
                    deadCells.emplace(cell);
                }
            }
            if(livingCells.empty()){
                continue;
            }
            while(!deadCells.empty()){
                auto father = fight(livingCells);
                auto mother = fight(livingCells);
                if(father.first == mother.first){
                    mother = fight(livingCells);
                }
                auto cell = deadCells.front();
                deadCells.pop();
                selected.emplace_back(father.first, mother.first, cell);
            }
        }
        return selected;
    }
}

#endif // EVOAI_TOURNAMENT_HPP