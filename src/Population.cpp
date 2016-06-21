#include <EvoAI/Population.hpp>

namespace EvoAI{
    Population::Population()
    : species()
    , genomes()
    , PopulationSize(50)
    , genomesCached(false){}
    Population::Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecurrent, bool cppn)
    : species()
    , genomes()
    , PopulationSize(size)
    , genomesCached(false){
        /** @todo spawn genomes and check species and add it to species.
        for(auto i=0u;i<size;++i){
            genomes.emplace_back(std::make_unique<Genome>(numInputs, numOutputs,canBeRecurrent,cppn));
            genomes[i]->setGenomeID(i);
        }
         */
    }
    Population::Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numOutputs, bool canBeRecurrent, bool cppn)
    : species()
    , genomes()
    , PopulationSize(size)
    , genomesCached(false){
        /** @todo
        for(auto i=0u;i<size;++i){
            genomes.emplace_back(std::make_unique<Genome>(numInputs, numOutputs,canBeRecurrent,cppn));
            genomes[i]->setGenomeID(i);
        }
         */
    }
    Population::Population(JsonBox::Object o)
    : species()
    , genomes()
    , PopulationSize(std::stoull(o["PopulationSize"].getString()))
    , genomesCached(false){
        auto specs = o["species"].getArray();
        for(auto& sp:specs){
            species.push_back(std::make_unique<Species>(sp.getObject()));
        }
    }
    Population::Population(const std::string& filename)
    : species()
    , genomes()
    , PopulationSize(50)
    , genomesCached(false){
        JsonBox::Value json;
        json.loadFromFile(filename);
        auto v = json["Population"];
        auto specs = v["species"].getArray();
        for(auto& sp:specs){
            species.push_back(std::make_unique<Species>(sp.getObject()));
        }
        PopulationSize = std::stoull(v["PopulationSize"].getString());
    }
    void Population::addGenome(std::unique_ptr<Genome> g) noexcept{
        /// @todo
    }
    void Population::removeGenome(Genome* g) noexcept{
        /// @todo
    }
    void Population::addSpecies(std::unique_ptr<Species> sp) noexcept{
        /// @todo
    }
    void Population::removeSpecies(Species* sp) noexcept{
        /// @todo
    }
    std::vector<Genome*>& Population::getGenomes() noexcept{
        /// @todo reimplement
        return genomes;
    }
    Genome* Population::findGenome(const std::size_t& id) noexcept{
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
    Genome* Population::getBestGenome() const noexcept{
        /// @todo reimplement
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
    void Population::speciate(const double& CompatibilityThreshold, const std::size_t& maxAge) noexcept{
        /** @todo reimplement
        species.erase(std::remove_if(std::begin(species),std::end(species),
                            [&](std::unique_ptr<Species>& sp){
                                if(!sp->isNovel()){
                                    sp->addAge(1);
                                }else{
                                    sp->setNovel(false);
                                }
                                if(sp->getAge() > maxAge){
                                    sp->setKillable(true);
                                }
                                if(sp->isKillable()){
                                    removeGenomesFromSpecies(sp->getID());
                                    return true;
                                }
                                return false;
                            }), std::end(species));
        for(auto& g:genomes){
            auto added = false;
            for(auto& sp:species){
                auto rep = sp->getRepresentative();
                if(rep){
                    auto dist = Genome::distance(*g,*rep);
                    if(dist >= -CompatibilityThreshold && dist <= CompatibilityThreshold){
                        added = true;
                        if(!sp->has(g.get())){
                            g->setSpeciesID(sp->getID());
                            sp->addGenome(g.get());
                        }
                        break;
                    }
                }
            }
            if(!added){
                auto currentSpeciesID = species.size();
                g->setSpeciesID(currentSpeciesID);
                auto sp = std::make_unique<Species>(currentSpeciesID,true);
                sp->addGenome(g.get());
                species.push_back(std::move(sp));
            }
        }
         */
    }
    void Population::reproduce(bool interSpecies) noexcept{
        /** @todo reimplement
        //speciate();
        if(interSpecies){
            orderGenomesByFitness();
            std::size_t half = (genomes.size()/2);
            for(auto i=half;i<genomes.size();++i){
                std::cout << "reproduce..." << std::endl;
                auto selectedFather = random(0,static_cast<int>(half));
                auto selectedMother = random(0,static_cast<int>(half));
                auto genID = genomes[i]->getGenomeID();
                if(genomes.size() < PopulationSize){
                    genomes.push_back(Genome::reproduce(*genomes[selectedFather],*genomes[selectedMother]));
                    genomes.back()->setGenomeID(genomes.size());
                    if(doAction(0.6)){
                        genomes.back()->mutate();
                    }
                }else{
                    genomes[i] = Genome::reproduce(*genomes[selectedFather],*genomes[selectedMother]);
                    genomes[i]->setGenomeID(genID);
                    if(doAction(0.6)){
                        genomes[i]->mutate();
                    }
                }
            }
        }else{
            
        }
        */
    }
    void Population::orderGenomesByFitness() noexcept{
        /// @todo reimplement
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
    JsonBox::Value Population::toJson() const noexcept{
        JsonBox::Object o;
        JsonBox::Array spcs;
        for(auto& sp:species){
            spcs.push_back(sp->toJson());
        }
        o["species"] = JsonBox::Value(spcs);
        o["PopulationSize"] = std::to_string(PopulationSize);
        return JsonBox::Value(o);
    }
    void Population::writeToFile(const std::string& filename){
        JsonBox::Value v;
        v["version"] = 1.0;
        v["Population"] = toJson();
        v.writeToFile(filename);
    }
}
