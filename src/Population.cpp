#include <EvoAI/Population.hpp>

namespace EvoAI{
    Population::Population()
    : genomes()
    , species(){}
    Population::Population(const std::size_t& numInputs, const std::size_t& numOutputs, const std::size_t& size, bool canBeRecurrent, bool cppn)
    : genomes()
    , species(){
        for(auto i=0u;i<size;++i){
            genomes.emplace_back(std::make_unique<Genome>(numInputs, numOutputs,canBeRecurrent,cppn));
            genomes[i]->setGenomeID(i);
        }
    }
    Population::Population(JsonBox::Object o)
    : genomes()
    , species(){
        auto gnms = o["genomes"].getArray();
        for(auto& g:gnms){
            genomes.push_back(std::make_unique<Genome>(g.getObject()));
        }
        auto specs = o["species"].getArray();
        for(auto& sp:specs){
            species.push_back(std::make_unique<Species>(sp.getObject()));
        }
        for(auto& g:genomes){
            auto sp = findSpecies(g->getSpeciesID());
            sp->addGenome(g.get());
        }
    }
    Population::Population(const std::string& filename)
    : genomes()
    , species(){
        JsonBox::Value json;
        json.loadFromFile(filename);
        auto v = json["Population"];
        auto gnms = v["genomes"].getArray();
        for(auto& g:gnms){
            genomes.push_back(std::make_unique<Genome>(g.getObject()));
        }
        auto specs = v["species"].getArray();
        for(auto& sp:specs){
            species.push_back(std::make_unique<Species>(sp.getObject()));
        }
        for(auto& g:genomes){
            auto sp = findSpecies(g->getSpeciesID());
            sp->addGenome(g.get());
        }
    }
    std::vector<std::unique_ptr<Genome>>& Population::getGenomes() noexcept{
        return genomes;
    }
    Genome* Population::findGenome(const std::size_t& id) noexcept{
        auto found = std::find_if(std::begin(genomes),std::end(genomes),
                                    [&](std::unique_ptr<Genome>& g){
                                        return (g->getGenomeID() == id);
                                    });
        if(found != std::end(genomes)){
            return (*found).get();
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
    std::vector<std::unique_ptr<Species>>& Population::getSpecies() noexcept{
        return species;
    }
    void Population::speciate(const double& CompatibilityThreshold) noexcept{
        /// @todo implement
        for(auto& g:genomes){
            auto added = false;
            for(auto& sp:species){
                auto rep = sp->getRepresentative();
                auto dist = Genome::distance(*g,*rep);
                if(dist > -CompatibilityThreshold && dist < CompatibilityThreshold){
                    added = true;
                    g->setSpeciesID(sp->getID());
                    sp->addGenome(g.get());
                    if(!sp->isNovel()){
                        sp->addAge(1);
                    }else{
                        sp->setNovel(false);
                    }
                    if(sp->getAge() > 15){
                        sp->setKillable(true);
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
    }
    void Population::reproduce(bool interSpecies) noexcept{
        /// @todo implement
        if(interSpecies){
            orderGenomesByFitness();
            for(auto& sp:species){
                sp->clear();
            }
            std::size_t half = (genomes.size()/2);
            for(auto i=half;i<genomes.size();++i){
                auto selectedFather = random(0,static_cast<int>(half));
                auto selectedMother = random(0,static_cast<int>(half));
                auto genID = genomes[i]->getGenomeID();
                genomes[i] = Genome::reproduce(*genomes[selectedFather],*genomes[selectedMother]);
                genomes[i]->setGenomeID(genID);
                if(doAction(0.6)){
                    genomes[i]->mutate();
                }
            }
        }else{
            
        }
    }
    void Population::orderGenomesByFitness() noexcept{
        std::sort(std::begin(genomes),std::end(genomes),
                    [](std::unique_ptr<Genome>& g1,std::unique_ptr<Genome>& g2){
                        return (g1->getFitness() > g2->getFitness());
                    });
    }
    void Population::orderSpeciesByAge() noexcept{
        speciate(6.0);
        std::sort(std::begin(species),std::end(species),
                        [](std::unique_ptr<Species>& s1, std::unique_ptr<Species>& s2){
                            return (s1->getAge() > s2->getAge());
                        });
    }
    void Population::orderSpeciesByFitness() noexcept{
        speciate(6.0);
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
    void Population::removeGenomesFromSpecies(const std::size_t& speciesID) noexcept{
        genomes.erase(std::remove_if(std::begin(genomes),std::end(genomes),
                            [&speciesID](std::unique_ptr<Genome>& g){
                                return (g->getSpeciesID() == speciesID);
                            }),std::end(genomes));
    }
    std::size_t Population::getGenomesSize() const noexcept{
        return genomes.size();
    }
    std::size_t Population::getSpeciesSize() const noexcept{
        return species.size();
    }
    JsonBox::Value Population::toJson() const noexcept{
        JsonBox::Object o;
        JsonBox::Array gnms;
        for(auto& g:genomes){
            gnms.push_back(g->toJson());
        }
        o["genomes"] = JsonBox::Value(gnms);
        JsonBox::Array spcs;
        for(auto& sp:species){
            spcs.push_back(sp->toJson());
        }
        o["species"] = JsonBox::Value(spcs);
        return JsonBox::Value(o);
    }
    void Population::writeToFile(const std::string& filename){
        JsonBox::Value v;
        v["version"] = 1.0;
        v["Population"] = toJson();
        v.writeToFile(filename);
    }
}
