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
            g->setGenomeID(i);
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
            g->setGenomeID(i);
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
    , genomesCached(false){
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
            auto currentSpeciesID = species.size();
            auto sp = std::make_unique<Species>(currentSpeciesID,true);
            g->setSpeciesID(currentSpeciesID);
            sp->addGenome(std::move(g));
            species.push_back(std::move(sp));
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
                                if(sp->getAge() > maxAge){
                                    sp->setKillable(true);
                                }
                                return sp->isKillable();
                            }), std::end(species));
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
