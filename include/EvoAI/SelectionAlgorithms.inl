namespace EvoAI{
    namespace SelectionAlgorithms{
        // truncation algo
        template<typename T>
        Truncation<T>::Truncation(std::size_t maxPop) noexcept
        : maxPopulation(maxPop){}
        template<typename T>
        std::vector<Selected<T>> Truncation<T>::operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept{
            std::sort(std::begin(members),std::end(members),
                            [](pointer g1, pointer g2){
                                return (g1->getFitness() > g2->getFitness());
                        });
            std::vector<Selected<T>> selected;
            selected.reserve(numberToSelect);
            std::size_t loserIndex = members.size() - 1;
            std::size_t half = std::floor(members.size() / 2);
            for(auto i=0u;i<numberToSelect;++i){
                auto selectedFather = randomGen().random(std::size_t(0),half);
                auto selectedMother = randomGen().random(std::size_t(0),half);
                selected.emplace_back(members[selectedFather], members[selectedMother], members[loserIndex--]);
            }
            return selected;
        }
        template<typename T>
        std::vector<Selected<T>> Truncation<T>::operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept{
            std::vector<Selected<T>> selected;
            selected.reserve(numberToSelect);
            for(auto& [id, sp]:species){
                sp->rank();
                std::size_t loserIndex = sp->getSize() - 1;
                std::size_t half = std::floor(sp->getSize() / 2);
                auto& spMembers = sp->getMembers();
                double percentage = sp->getSize() / static_cast<double>(maxPopulation);
                std::size_t numToSelectPerSpecies = std::floor(numberToSelect * percentage);
                for(auto i=0u;i<numToSelectPerSpecies;++i){
                    auto selectedFather = randomGen().random(std::size_t(0), half);
                    auto selectedMother = randomGen().random(std::size_t(0), half);
                    if constexpr(std::is_pointer_v<T>){
                        selected.emplace_back(spMembers[selectedFather], spMembers[selectedMother], spMembers[loserIndex]);
                    }else{
                        selected.emplace_back(&spMembers[selectedFather], &spMembers[selectedMother], &spMembers[loserIndex]);
                    }
                    if(loserIndex == 0) break;
                    --loserIndex;
                }
            }
            return selected;
        }
        // tournament algo
            template<typename T>
            Tournament<T>::Tournament(std::size_t maxPopulation, std::size_t rnds) noexcept
            : maxPop(maxPopulation)
            , rounds(rnds){}
            template<typename T>
            template<typename Members>
            std::pair<typename Tournament<T>::pointer, typename Tournament<T>::pointer> Tournament<T>::fight(Members& members) noexcept{
                std::size_t max = members.size() - 1;
                pointer champ = nullptr;
                pointer loser = nullptr;
                if constexpr(std::is_pointer_v<T> || std::is_same_v<Members, std::vector<pointer>>){
                    champ = members[randomGen().random(std::size_t(0), max)];
                    loser = members[randomGen().random(std::size_t(0), max)];
                }else{
                    champ = &members[randomGen().random(std::size_t(0), max)];
                    loser = &members[randomGen().random(std::size_t(0), max)];
                }
                for(auto i=0u;i<rounds;++i){
                    pointer contender = nullptr;
                    if constexpr(std::is_pointer_v<T> || std::is_same_v<Members, std::vector<pointer>>){
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
            template<typename T>
            std::vector<Selected<T>> Tournament<T>::operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept{
                std::vector<Selected<T>> selected;
                selected.reserve(numberToSelect);
                std::unordered_map<std::size_t, pointer> alreadyALoser;
                alreadyALoser.reserve(numberToSelect);
                for(auto i=0u;i<numberToSelect;++i){
                    auto father = fight(members);
                    auto mother = fight(members);
                    if(father.first == mother.first){
                        mother = fight(members);
                    }
                    if(alreadyALoser.find(father.second->getID()) == std::end(alreadyALoser)){
                        selected.emplace_back(father.first, mother.first, father.second);
                        alreadyALoser.insert(std::make_pair(father.second->getID(), father.second));
                    }else if(alreadyALoser.find(mother.second->getID()) == std::end(alreadyALoser)){
                        selected.emplace_back(father.first, mother.first, mother.second);
                        alreadyALoser.insert(std::make_pair(mother.second->getID(), mother.second));
                    }
                }
                return selected;
            }
            template<typename T>
            std::vector<Selected<T>> Tournament<T>::operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept{
                std::vector<Selected<T>> selected;
                selected.reserve(numberToSelect);
                for(auto& [id, sp]: species){
                    std::unordered_map<std::size_t, pointer> alreadyALoser;
                    double percentage = sp->getSize() / static_cast<double>(maxPop);
                    std::size_t numToSelectPerSpecies = std::floor(numberToSelect * percentage);
                    alreadyALoser.reserve(numToSelectPerSpecies);
                    auto& members = sp->getMembers();
                    for(auto i=0u;i<numToSelectPerSpecies;++i){
                        auto father = fight(members);
                        auto mother = fight(members);
                        if(father.first == mother.first){
                            mother = fight(members);
                        }
                        if(alreadyALoser.find(father.second->getID()) == std::end(alreadyALoser)){
                            selected.emplace_back(father.first, mother.first, father.second);
                            alreadyALoser.insert(std::make_pair(father.second->getID(), father.second));
                        }else if(alreadyALoser.find(mother.second->getID()) == std::end(alreadyALoser)){
                            selected.emplace_back(father.first, mother.first, mother.second);
                            alreadyALoser.insert(std::make_pair(mother.second->getID(), mother.second));
                        }
                    }
                }
                return selected;
            }
            // FPS algo
            template<typename T>
            FPS<T>::FPS(std::size_t maxPopulation) noexcept
            : maxPop(maxPopulation){}
            template<typename T>
            template<typename Members>
            typename FPS<T>::pointer FPS<T>::FPSelection(Members& members, double totalFitness) noexcept{
                double r = randomGen().random(-1.0, 1.0);
                double covered = 0.0;
                for(auto& m:members){
                    if constexpr(std::is_pointer_v<T> || std::is_same_v<Members, std::vector<pointer>>){
                        covered += (m->getFitness() / totalFitness);
                        if(covered > r){
                            return m;
                        }
                    }else{
                        covered += (m.getFitness() / totalFitness);
                        if(covered > r){
                            return &m;
                        }
                    }
                }
                return nullptr;
            }
            template<typename T>
            std::vector<Selected<T>> FPS<T>::operator()(std::vector<std::remove_pointer_t<T>*>& members, std::size_t numberToSelect) noexcept{
                std::vector<Selected<T>> selected;
                selected.reserve(numberToSelect);
                std::unordered_map<std::size_t, pointer> alreadyALoser;
                alreadyALoser.reserve(numberToSelect);
                double totalFitness = std::accumulate(std::begin(members), std::end(members), 0.0, 
                    [](auto& a, auto& b){
                        return a + b->getFitness();
                });
                for(auto i=0u;i<numberToSelect;++i){
                    auto father = FPSelection(members, totalFitness);
                    auto mother = FPSelection(members, totalFitness);
                    auto loser = FPSelection(members, totalFitness);
                    if(father && mother && loser){
                        if(alreadyALoser.find(loser->getID()) == std::end(alreadyALoser)){
                            selected.emplace_back(father, mother, loser);
                            alreadyALoser.insert(std::make_pair(loser->getID(), loser));
                        }
                    }
                }
                return selected;
            }
            template<typename T>
            std::vector<Selected<T>> FPS<T>::operator()(std::map<std::size_t, std::unique_ptr<Species<T>>>& species, std::size_t numberToSelect) noexcept{
                std::vector<Selected<T>> selected;
                selected.reserve(numberToSelect);
                for(auto& [id, sp]: species){
                    std::unordered_map<std::size_t, pointer> alreadyALoser;
                    double percentage = sp->getSize() / static_cast<double>(maxPop);
                    std::size_t numToSelectPerSpecies = std::floor(numberToSelect * percentage);
                    alreadyALoser.reserve(numToSelectPerSpecies);
                    auto& members = sp->getMembers();
                    double totalFitness = 0.0;
                    if constexpr(std::is_pointer_v<T>){
                        totalFitness = std::accumulate(std::begin(members), std::end(members),0.0, 
                            [](auto& a, auto& b){
                                return a + b->getFitness();
                        });
                    }else{
                        totalFitness = std::accumulate(std::begin(members), std::end(members),0.0, 
                            [](auto& a, auto& b){
                                return a + b.getFitness();
                        });
                    }
                    for(auto i=0u;i<numToSelectPerSpecies;++i){
                        auto father = FPSelection(members, totalFitness);
                        auto mother = FPSelection(members, totalFitness);
                        auto loser = FPSelection(members, totalFitness);
                        if(father && mother && loser){
                            if(alreadyALoser.find(loser->getID()) == std::end(alreadyALoser)){
                                selected.emplace_back(father, mother, loser);
                                alreadyALoser.insert(std::make_pair(loser->getID(), loser));
                            }
                        }
                    }
                }
                return selected;
            }
    }
}