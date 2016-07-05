#include <EvoAI/Population.hpp>

namespace EvoAI{
    Population::Population()
    : species()
    , genomes()
    , PopulationSize(50)
    , genomesCached(false)
    , compatibilityThreshold(6.0)
    , maxAge(120){}
    Population::Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numOutputs, bool canBeRecurrent, bool cppn)
    : species()
    , genomes()
    , PopulationSize(size)
    , genomesCached(false)
    , compatibilityThreshold(6.0)
    , maxAge(120){
        for(auto i=0u;i<size;++i){
            auto g = std::make_unique<Genome>(numInputs, numOutputs, canBeRecurrent, cppn);
            g->setGenomeID(Population::getNewID());
            addGenome(std::move(g));
        }
    }
    Population::Population(const std::size_t& size, const std::size_t& numInputs, const std::size_t& numHidden, const std::size_t& numOutputs, bool canBeRecurrent, bool cppn)
    : species()
    , genomes()
    , PopulationSize(size)
    , genomesCached(false)
    , compatibilityThreshold(6.0)
    , maxAge(120){
        for(auto i=0u;i<size;++i){
            auto g = std::make_unique<Genome>(numInputs, numHidden, numOutputs, canBeRecurrent, cppn);
            g->setGenomeID(Population::getNewID());
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
    , maxAge(120){
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
            static auto currentSpeciesID = 0;
            auto sp = std::make_unique<Species>(currentSpeciesID,true);
            g->setSpeciesID(currentSpeciesID);
            sp->addGenome(std::move(g));
            species.push_back(std::move(sp));
            ++currentSpeciesID;
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
                                    sp->computeAvgFitness();
                                    if(sp->isStagnant()){
                                        sp->addAge(2);
                                    }else{
                                        sp->addAge(1);
                                    }
                                }else{
                                    sp->setNovel(false);
                                }
                                if((sp->getAge() > maxAge) || (sp->getGenomes().size() <= 1)){
                                    sp->setKillable(true);
                                }
                                return sp->isKillable();
                            }), std::end(species));
    }
    void Population::reproduce(bool interSpecies, Population::SelectionType st) noexcept{
        auto numOffsprings = 0u;
        if(species.size() > 1){
            removeOldSpecies();
            numOffsprings = ((PopulationSize - getGenomes().size()) / species.size());
        }
        if(interSpecies){
            switch(st){
                case Population::SelectionType::TRUNCATION:{
                        std::vector<std::unique_ptr<Genome>> genomesToAdd;
                        orderGenomesByFitness();
                        std::size_t size = genomes.size();
                        std::size_t half = (size / 2);
                        if(numOffsprings > 0){
                            for(auto i=0u;i<numOffsprings;++i){
                                auto selectedFather = random(0,half);
                                auto selectedMother = random(0,half);
                                auto child = Genome::reproduce(*genomes[selectedFather],*genomes[selectedMother]);
                                child->setGenomeID(Population::getNewID());
                                genomesToAdd.push_back(std::move(child));
                            }
                        }
                        std::vector<Genome*> genomesToRemove;
                        for(auto i=half;i<size;++i){
                            auto selectedFather = random(0,half);
                            auto selectedMother = random(0,half);
                            auto child = Genome::reproduce(*genomes[selectedFather],*genomes[selectedMother]);
                            auto oldID = genomes[i]->getGenomeID();
                            child->setGenomeID(oldID);
                            genomesToAdd.push_back(std::move(child));
                            genomesToRemove.push_back(genomes[i]);
                        }
                        for(auto& g:genomesToRemove){
                            auto sp = findSpecies(g->getSpeciesID());
                            if(sp){
                                sp->removeGenome(g);
                            }
                        }
                        for(auto& g:genomesToAdd){
                            addGenome(std::move(g));
                        }
                }   break;
                case Population::SelectionType::TOURNAMENT:{
                        std::vector<std::unique_ptr<Genome>> genomesToAdd;
                        auto tournamentSelection = [&](std::size_t rounds){
                            int champ = -1;
                            int looser = -1;
                            for(auto i=0u;i<rounds;++i){
                                auto contender = random(0,genomes.size()-1);
                                if(champ == -1){
                                    champ = contender;
                                    looser = contender;
                                }else if(genomes[contender]->getFitness() > genomes[champ]->getFitness()){
                                    looser = champ;
                                    champ = contender;
                                }
                            }
                            return std::make_pair(champ,looser);
                        };
                        if(numOffsprings > 0){
                            for(auto i=0u;i<numOffsprings;++i){
                                auto father = tournamentSelection(3);
                                auto mother = tournamentSelection(3);
                                auto child = Genome::reproduce(*genomes[father.first],*genomes[mother.first]);
                                child->setGenomeID(Population::getNewID());
                                genomesToAdd.push_back(std::move(child));
                            }
                        }
                        std::vector<Genome*> genomesToRemove;
                        std::size_t half = (genomes.size() / 2);
                        for(auto i=0u;i<half;++i){
                            auto father = tournamentSelection(3);
                            auto mother = tournamentSelection(3);
                            auto newChild = mother.second;
                            auto found = std::find(std::begin(genomesToRemove),std::end(genomesToRemove),genomes[newChild]);
                            if(found != std::end(genomesToRemove)){
                                continue;
                            }
                            auto child = Genome::reproduce(*genomes[father.first],*genomes[mother.first]);
                            auto oldID = genomes[newChild]->getGenomeID();
                            child->setGenomeID(oldID);
                            genomesToAdd.push_back(std::move(child));
                            genomesToRemove.push_back(genomes[newChild]);
                        }
                        for(auto& g:genomesToRemove){
                            auto sp = findSpecies(g->getSpeciesID());
                            if(sp){
                                sp->removeGenome(g);
                            }
                        }
                        for(auto& g:genomesToAdd){
                            addGenome(std::move(g));
                        }
                }   break;
                case Population::SelectionType::FPS:
                default:{
                        std::vector<std::unique_ptr<Genome>> genomesToAdd;
                        orderGenomesByFitness();
                        auto totalFitness = 0.0;
                        for(auto& g:genomes){
                            totalFitness += g->getFitness();
                        }
                        auto fpSelection = [&](double totalFitness) -> Genome*{
                            auto r = random(0.0,1.0);
                            auto covered = 0.0;
                            for(auto& g:genomes){
                                covered += (g->getFitness()/totalFitness);
                                if(r<covered){
                                    return g;
                                }
                            }
                            return nullptr;
                        };
                        if(numOffsprings > 0){
                            for(auto i=0u;i<numOffsprings;++i){
                                auto father = fpSelection(totalFitness);
                                auto mother = fpSelection(totalFitness);
                                if(father && mother){
                                    auto child = Genome::reproduce(*father,*mother);
                                    child->setGenomeID(Population::getNewID());
                                    genomesToAdd.push_back(std::move(child));
                                }
                            }
                        }
                        std::vector<Genome*> genomesToRemove;
                        auto half = (genomes.size() / 2);
                        for(auto i=half;i<genomes.size();++i){
                            auto father = fpSelection(totalFitness);
                            auto mother = fpSelection(totalFitness);
                            auto found = std::find(std::begin(genomesToRemove),std::end(genomesToRemove),genomes[i]);
                            if(found != std::end(genomesToRemove)){
                                continue;
                            }
                            if(father && mother){
                                auto child = Genome::reproduce(*father,*mother);
                                auto oldID = genomes[i]->getGenomeID();
                                child->setGenomeID(oldID);
                                genomesToAdd.push_back(std::move(child));
                                genomesToRemove.push_back(genomes[i]);
                            }
                        }
                        for(auto& g:genomesToRemove){
                            auto sp = findSpecies(g->getSpeciesID());
                            if(sp){
                                sp->removeGenome(g);
                            }
                        }
                        for(auto& g:genomesToAdd){
                            addGenome(std::move(g));
                        }
                }    break;
            }
        }else{
            switch(st){
                case Population::SelectionType::TRUNCATION:{
                        std::vector<std::unique_ptr<Genome>> genomesToAdd;
                        for(auto& sp:species){
                            sp->rank();
                            auto& spGenomes = sp->getGenomes();
                            std::size_t size = spGenomes.size();
                            std::size_t half = size / 2;
                            if(numOffsprings > 0){
                                for(auto i=0u;i<numOffsprings;++i){
                                    auto selectedFather = random(0,half);
                                    auto selectedMother = random(0,half);
                                    auto child = Genome::reproduce(*spGenomes[selectedFather],*spGenomes[selectedMother]);
                                    child->setGenomeID(Population::getNewID());
                                    genomesToAdd.push_back(std::move(child));
                                }
                            }
                            std::vector<Genome*> genomesToRemove;
                            for(auto i=half;i<size;++i){
                                auto selectedFather = random(0,half);
                                auto selectedMother = random(0,half);
                                auto child = Genome::reproduce(*spGenomes[selectedFather],*spGenomes[selectedMother]);
                                auto oldID = spGenomes[i]->getGenomeID();
                                child->setGenomeID(oldID);
                                genomesToAdd.push_back(std::move(child));
                                genomesToRemove.push_back(spGenomes[i].get());
                            }
                            for(auto& g:genomesToRemove){
                                sp->removeGenome(g);
                            }
                        }
                        for(auto& g:genomesToAdd){
                            addGenome(std::move(g));
                        }
                }   break;
                case Population::SelectionType::TOURNAMENT:{
                        std::vector<std::unique_ptr<Genome>> genomesToAdd;
                        for(auto& sp: species){
                            auto& spGenomes = sp->getGenomes();
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
                            if(numOffsprings > 0){
                                for(auto i=0u;i<numOffsprings;++i){
                                    auto father = tournamentSelection(3);
                                    auto mother = tournamentSelection(3);
                                    auto child = Genome::reproduce(*spGenomes[father.first],*spGenomes[mother.first]);
                                    child->setGenomeID(Population::getNewID());
                                    genomesToAdd.push_back(std::move(child));
                                }
                            }
                            std::vector<Genome*> genomesToRemove;
                            auto half = (spGenomes.size() / 2);
                            for(auto i=0u;i<half;++i){
                                auto father = tournamentSelection(3);
                                auto mother = tournamentSelection(3);
                                auto newChild = mother.second;
                                auto found = std::find(std::begin(genomesToRemove),std::end(genomesToRemove),spGenomes[newChild].get());
                                if(found != std::end(genomesToRemove)){
                                    continue;
                                }
                                auto child = Genome::reproduce(*spGenomes[father.first],*spGenomes[mother.first]);
                                auto oldID = spGenomes[newChild]->getGenomeID();
                                child->setGenomeID(oldID);
                                genomesToAdd.push_back(std::move(child));
                                genomesToRemove.push_back(spGenomes[newChild].get());
                            }
                            for(auto& g:genomesToRemove){
                                sp->removeGenome(g);
                            }
                        }
                        for(auto& g:genomesToAdd){
                            addGenome(std::move(g));
                        }
                }   break;
                case Population::SelectionType::FPS:
                default:{
                        std::vector<std::unique_ptr<Genome>> genomesToAdd;
                        for(auto& sp:species){
                            sp->rank();
                            auto totalFitness = 0.0;
                            auto& spGenomes = sp->getGenomes();
                            for(auto& g:spGenomes){
                                totalFitness += g->getFitness();
                            }
                            auto fpSelection = [&](double totalFitness) -> Genome*{
                                auto r = random(0.0,1.0);
                                auto covered = 0.0;
                                for(auto& g:spGenomes){
                                    covered += (g->getFitness()/totalFitness);
                                    if(r<covered){
                                        return g.get();
                                    }
                                }
                                return nullptr;
                            };
                            if(numOffsprings > 0){
                                for(auto i=0u;i<numOffsprings;++i){
                                    auto father = fpSelection(totalFitness);
                                    auto mother = fpSelection(totalFitness);
                                    if(father && mother){
                                        auto child = Genome::reproduce(*father,*mother);
                                        child->setGenomeID(Population::getNewID());
                                        genomesToAdd.push_back(std::move(child));
                                    }
                                }
                            }
                            std::vector<Genome*> genomesToRemove;
                            auto half = (spGenomes.size() / 2);
                            for(auto i=half;i<spGenomes.size();++i){
                                auto father = fpSelection(totalFitness);
                                auto mother = fpSelection(totalFitness);
                                auto found = std::find(std::begin(genomesToRemove),std::end(genomesToRemove),spGenomes[i].get());
                                if(found != std::end(genomesToRemove)){
                                    continue;
                                }
                                if(father && mother){
                                    auto child = Genome::reproduce(*father,*mother);
                                    auto oldID = spGenomes[i]->getGenomeID();
                                    child->setGenomeID(oldID);
                                    genomesToAdd.push_back(std::move(child));
                                    genomesToRemove.push_back(spGenomes[i].get());
                                }
                            }
                            for(auto& g:genomesToRemove){
                                sp->removeGenome(g);
                            }
                        }
                        for(auto& g:genomesToAdd){
                            addGenome(std::move(g));
                        }
                }    break;
            }
        }
        genomesCached = false;
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
    double Population::computeAvgFitness() noexcept{
        if(!genomesCached){
            getGenomes();
        }
        double avgFitness = 0.0;
        for(auto& g:genomes){
            avgFitness += g->getFitness();
        }
        return (avgFitness/genomes.size());
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
//////////////
///// private
//////////////
    std::size_t Population::getNewID() noexcept{
        static auto currentID = 0;
        return currentID++;
    }
}
