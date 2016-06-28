#include <EvoAI/Population.hpp>

namespace EvoAI{
    Population::Population()
    : species()
    , genomes()
    , PopulationSize(50)
    , genomesCached(false)
    , compatibilityThreshold(6.0)
    , maxAge(20){}
    Population::Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecurrent, bool cppn)
    : species()
    , genomes()
    , PopulationSize(size)
    , genomesCached(false)
    , compatibilityThreshold(6.0)
    , maxAge(20){
        for(auto i=0u;i<size;++i){
            auto g = std::make_unique<Genome>(numInputs, numOutputs, canBeRecurrent, cppn);
            g->setGenomeID(getNewID());
            addGenome(std::move(g));
        }
    }
    Population::Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numOutputs, bool canBeRecurrent, bool cppn)
    : species()
    , genomes()
    , PopulationSize(size)
    , genomesCached(false)
    , compatibilityThreshold(6.0)
    , maxAge(20){
        for(auto i=0u;i<size;++i){
            auto g = std::make_unique<Genome>(numInputs, numHidden, numOutputs, canBeRecurrent, cppn);
            g->setGenomeID(getNewID());
            addGenome(std::move(g));
        }
    }
    Population::Population(JsonBox::Object o)
    : species()
    , genomes()
    , PopulationSize(std::stoull(o["PopulationSize"].getString()))
    , genomesCached(false)
    , compatibilityThreshold(o["compatibilityThreshold"].getDouble())
    , maxAge(std::stoull(o["maxAge"].getString())){
        auto specs = o["species"].getArray();
        for(auto& sp:specs){
            species.push_back(std::make_unique<Species>(sp.getObject()));
        }
    }
    Population::Population(const std::string& filename)
    : species()
    , genomes()
    , PopulationSize(50)
    , genomesCached(false)
    , compatibilityThreshold(6.0)
    , maxAge(20){
        JsonBox::Value json;
        json.loadFromFile(filename);
        auto v = json["Population"];
        auto specs = v["species"].getArray();
        for(auto& sp:specs){
            species.push_back(std::make_unique<Species>(sp.getObject()));
        }
        PopulationSize = std::stoull(v["PopulationSize"].getString());
        compatibilityThreshold = v["compatibilityThreshold"].getDouble();
        maxAge = std::stoull(v["maxAge"].getString());
    }
    void Population::addGenome(std::unique_ptr<Genome> g) noexcept{
        std::cout << "\t\tAddGenome" << std::endl; /// @todo remove
        genomesCached = false;
        auto added = false;
        for(auto& sp:species){
            auto rep = sp->getRepresentative();
            if(rep){
                auto dist = Genome::distance(*g,*rep);
                if(dist >= -compatibilityThreshold && dist <= compatibilityThreshold){
                    added = true;
                    g->setSpeciesID(sp->getID());
                    sp->addGenome(std::move(g));
                    break;
                }
            }
        }
        if(!added){
            static auto currentSpeciesID = 0;
            auto sp = std::make_unique<Species>(currentSpeciesID,true);
            g->setSpeciesID(currentSpeciesID);
            sp->addGenome(std::move(g));
            species.push_back(std::move(sp));
            currentSpeciesID++;
        }
    }
    void Population::removeGenome(Genome* g) noexcept{
        auto sp = findSpecies(g->getSpeciesID());
        sp->removeGenome(g);
        genomesCached = false;
    }
    void Population::addSpecies(std::unique_ptr<Species> sp) noexcept{
        species.push_back(std::move(sp));
        genomesCached = false;
    }
    void Population::removeSpecies(Species* sp) noexcept{
        species.erase(std::remove_if(std::begin(species),std::end(species),
                                    [&](std::unique_ptr<Species>& sp2){
                                        return (sp2.get() == sp);
                                    }),std::end(species));
        genomesCached = false;
    }
    std::vector<Genome*>& Population::getGenomes() noexcept{
        if(genomesCached){
            return genomes;
        }
        genomesCached = true;
        genomes.clear();
        for(auto& sp:species){
            for(auto& g:sp->getGenomes()){
                genomes.push_back(g.get());
            }
        }
        return genomes;
    }
    Genome* Population::findGenome(const std::size_t& id) noexcept{
        if(!genomesCached){
            getGenomes();
        }
        auto found = std::find_if(std::begin(genomes),std::end(genomes),
                                    [&](Genome* g){
                                        return (g->getGenomeID() == id);
                                    });
        if(found != std::end(genomes)){
            return (*found);
        }
        return nullptr;
    }
    Species* Population::findSpecies(const std::size_t& id) noexcept{
        auto found = std::find_if(std::begin(species),std::end(species),
                                    [&](std::unique_ptr<Species>& s){
                                        return (s->getID() == id);
                                    });
        if(found != std::end(species)){
            return (*found).get();
        }
        return nullptr;
    }
    Genome* Population::getBestGenome() noexcept{
        if(!genomesCached){
            getGenomes();
        }
        Genome* best = nullptr;
        auto fitness = 0.0;
        for(auto& g:genomes){
            if(g->getFitness() > fitness){
                fitness = g->getFitness();
                best = g;
            }
        }
        return best;
    }
    std::vector<std::unique_ptr<Species>>& Population::getSpecies() noexcept{
        return species;
    }
    void Population::removeOldSpecies() noexcept{
        genomesCached = false;
        species.erase(std::remove_if(std::begin(species),std::end(species),
                            [&](std::unique_ptr<Species>& sp){
                                if(!sp->isNovel()){
                                    sp->addAge(1);
                                }else{
                                    sp->setNovel(false);
                                }
                                if((sp->getAge() > maxAge) || (sp->getGenomes().size() < 2)){
                                    sp->setKillable(true);
                                }
                                std::cout << "\tID: " << std::to_string(sp->getID()) << "\tKilled: " << (sp->isKillable() ? "TRUE":"False") << std::endl; /// @todo remove
                                return sp->isKillable();
                            }), std::end(species));
    }
    std::size_t Population::getNewID() const noexcept{
        static auto currentID = 0;
        return currentID++;
    }
    void Population::reproduce(bool interSpecies, Population::SelectionType st) noexcept{
        removeOldSpecies();
        auto numOffsprings = ((PopulationSize - getGenomes().size()) / species.size());
        std::cout << "\t\t numOff: " << numOffsprings << std::endl; /// @todo remove
        if(interSpecies){
            
        }else{
            switch(st){
                case Population::SelectionType::TRUNCATION:{
                        std::vector<std::unique_ptr<Genome>> temp;
                        for(auto& sp:species){
                            sp->rank();
                            auto& spGenomes = sp->getGenomes();
                            auto fatherCounter = 0u;
                            auto motherCounter = 1u;
                            for(auto i=0u;i<numOffsprings;++i){
                                auto child = Genome::reproduce(*spGenomes[fatherCounter],*spGenomes[motherCounter]);
                                if(doAction(0.6)){
                                    child->mutate();
                                }
                                child->setGenomeID(getNewID());
                                temp.push_back(std::move(child));
                                if(fatherCounter < spGenomes.size()){
                                    ++fatherCounter;
                                }else{
                                    fatherCounter = 0;
                                }
                                if(motherCounter < spGenomes.size()){
                                    ++motherCounter;
                                }else{
                                    motherCounter = 1;
                                }
                            }
                            auto half = (spGenomes.size() / 2) - 1;
                            auto halfCounter = half;
                            for(auto i=0u;i<half;++i){
                                auto child = Genome::reproduce(*spGenomes[i],*spGenomes[i+1]);
                                auto oldID = spGenomes[halfCounter]->getGenomeID();
                                child->setGenomeID(oldID);
                                spGenomes[halfCounter] = std::move(child);
                                if(halfCounter < spGenomes.size()){
                                    ++halfCounter;
                                }
                            }
                        }
                        for(auto i=0u;i<temp.size();++i){
                            addGenome(std::move(temp[i]));
                        }
                }   break;
                case Population::SelectionType::TOURNAMENT:{
                        std::vector<std::unique_ptr<Genome>> temp;
                        std::cout << "------------ SP SIZE: "<< species.size() << std::endl; /// @todo remove
                        for(auto& sp: species){
                            std::cout << "\t------------ SP ID: " << std::to_string(sp->getID()) << std::endl; /// @todo remove
                            auto& spGenomes = sp->getGenomes();
                            sp->adjustFitness();
                            auto tournamentSelection = [&](std::size_t rounds){
                                int champ = -1;
                                int looser = -1;
                                for(auto i=0u;i<rounds;++i){
                                    auto contender = random(0,spGenomes.size()-1);
                                    if(champ == -1){
                                        champ = contender;
                                        looser = contender;
                                    }else if(spGenomes[contender]->getFitness() > spGenomes[champ]->getFitness()){
                                        looser = champ;
                                        champ = contender;
                                    }
                                }
                                return std::make_pair(champ,looser);
                            };
                            for(auto i=0u;i<numOffsprings;++i){
                                auto father = tournamentSelection(2);
                                auto mother = tournamentSelection(2);
                                auto child = Genome::reproduce(*spGenomes[father.first],*spGenomes[mother.first]);
                                if(doAction(0.6)){
                                    child->mutate();
                                }
                                std::cout << "\t\tAdding"<< std::endl; /// @todo remove
                                child->setGenomeID(getNewID());
                                temp.push_back(std::move(child));
                            }
                            auto spSize = (spGenomes.size());
                            for(auto i=0u;i<(spSize / 2);++i){
                                auto father = tournamentSelection(2);
                                auto mother = tournamentSelection(2);
                                auto newChild = mother.second;
                                auto child = Genome::reproduce(*spGenomes[father.first],*spGenomes[mother.first]);
                                if(doAction(0.6)){
                                    child->mutate();
                                }
                                std::cout << "\t\tReplacing"<< std::endl; /// @todo remove
                                auto oldID = spGenomes[newChild]->getGenomeID();
                                child->setGenomeID(oldID);
                                temp.push_back(std::move(child));
                                sp->removeGenome(spGenomes[newChild].get());
                            }
                        }
                        for(auto i=0u;i<temp.size();++i){
                            std::cout << "temp Adding" << std::endl; /// @todo remove
                            addGenome(std::move(temp[i]));
                        }
                }   break;
                case Population::SelectionType::FPS:{ /// @todo
                }    break;
                case Population::SelectionType::SUS:
                default:{ /// @todo
                    
                }   break;
            }
        }
    }
    void Population::orderGenomesByFitness() noexcept{
        if(!genomesCached){
            getGenomes();
        }
        std::sort(std::begin(genomes),std::end(genomes),
                        [](Genome* g1, Genome* g2){
                            return (g1->getFitness() > g2->getFitness());
                    });
    }
    void Population::orderSpeciesByAge() noexcept{
        std::sort(std::begin(species),std::end(species),
                        [](std::unique_ptr<Species>& s1, std::unique_ptr<Species>& s2){
                            return (s1->getAge() > s2->getAge());
                        });
    }
    void Population::orderSpeciesByFitness() noexcept{
        /// @todo computeAvg...?
        std::sort(std::begin(species),std::end(species),
                        [](std::unique_ptr<Species>& s1, std::unique_ptr<Species>& s2){
                            s1->computeAvgFitness();
                            s2->computeAvgFitness();
                            return (s1->getAvgFitness() > s2->getAvgFitness());
                        });
    }
    void Population::rankWithinSpecies() noexcept{
        for(auto& sp:species){
            sp->rank();
        }
    }
    std::size_t Population::getPopulationSize() const noexcept{
        return PopulationSize;
    }
    std::size_t Population::getSpeciesSize() const noexcept{
        return species.size();
    }
    void Population::setMaxAge(std::size_t age) noexcept{
        maxAge = age;
    }
    const std::size_t& Population::getMaxAge() const noexcept{
        return maxAge;
    }
    void Population::setCompatibilityThreshold(const double& compThreshold) noexcept{
        compatibilityThreshold = compThreshold;
    }
    const double& Population::getCompatibilityThreshold() const noexcept{
        return compatibilityThreshold;
    }
    JsonBox::Value Population::toJson() const noexcept{
        JsonBox::Object o;
        JsonBox::Array spcs;
        for(auto& sp:species){
            spcs.push_back(sp->toJson());
        }
        o["species"] = JsonBox::Value(spcs);
        o["PopulationSize"] = std::to_string(PopulationSize);
        o["compatibilityThreshold"] = compatibilityThreshold;
        o["maxAge"] = std::to_string(maxAge);
        return JsonBox::Value(o);
    }
    void Population::writeToFile(const std::string& filename){
        JsonBox::Value v;
        v["version"] = 1.0;
        v["Population"] = toJson();
        v.writeToFile(filename);
    }
}
