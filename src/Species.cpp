#include <EvoAI/Species.hpp>

#include <algorithm>

namespace EvoAI{
    Species::Species()
    : id(0)
    , age(0)
    , novel(true)
    , killable(false)
    , avgFitness(0.0)
    , maxFitness(0.0)
    , oldAvgFitness(0.0)
    , genomes(){}
    Species::Species(std::size_t id, bool novel)
    : id(id)
    , age(0)
    , novel(novel)
    , killable(false)
    , avgFitness(0.0)
    , maxFitness(0.0)
    , oldAvgFitness(0.0)
    , genomes(){}
    Species::Species(JsonBox::Object o)
    : id(std::stoull(o["id"].getString()))
    , age(std::stoull(o["age"].getString()))
    , novel(o["novel"].getBoolean())
    , killable(o["killable"].getBoolean())
    , avgFitness(o["avgFitness"].getDouble())
    , maxFitness(o["maxFitness"].getDouble())
    , oldAvgFitness(o["oldAvgFitness"].getDouble())
    , genomes(){
        auto gnms = o["genomes"].getArray();
        for(auto& g:gnms){
            genomes.push_back(std::make_unique<Genome>(g.getObject()));
        }
    }
    Species::Species(const std::string& filename)
    : id(0)
    , age(0)
    , novel(true)
    , killable(false)
    , avgFitness(0.0)
    , maxFitness(0.0)
    , oldAvgFitness(0.0)
    , genomes(){
        JsonBox::Value json;
        json.loadFromFile(filename);
        auto v = json["Species"];
        id = std::stoull(v["id"].getString());
        age = std::stoull(v["age"].getString());
        novel = v["novel"].getBoolean();
        killable = v["killable"].getBoolean();
        avgFitness = v["avgFitness"].getDouble();
        maxFitness = v["maxFitness"].getDouble();
        oldAvgFitness = v["oldAvgFitness"].getDouble();
        auto gnms = v["genomes"].getArray();
        for(auto& g:gnms){
            genomes.push_back(std::make_unique<Genome>(g.getObject()));
        }
    }
    void Species::adjustFitness() noexcept{
        auto size = (genomes.size() - 1);
        for(auto& g1:genomes){
            g1->setFitness(g1->getFitness()/size);
        }
    }
    void Species::rank() noexcept{
        std::sort(std::begin(genomes),std::end(genomes),
                    [](std::unique_ptr<Genome>& g1, std::unique_ptr<Genome>& g2){
                        return (g1->getFitness() > g2->getFitness());
                    });
    }
    Genome* Species::getRepresentative() const noexcept{
        if(!genomes.empty()){
            return genomes[0].get();
        }
        return nullptr;
    }
    Genome* Species::getChampion() noexcept{
        Genome* champ = nullptr;
        auto fitness = 0.0;
        for(auto& g:genomes){
            if(g->getFitness() > fitness){
                champ = g.get();
                fitness = g->getFitness();
            }
        }
        return champ;
    }
    bool Species::isNovel() const noexcept{
        return novel;
    }
    void Species::setNovel(bool n) noexcept{
        novel = n;
    }
    bool Species::isKillable() const noexcept{
        return killable;
    }
    void Species::setKillable(bool k) noexcept{
        killable = k;
    }
    void Species::setID(const std::size_t& speciesID) noexcept{
        id = speciesID;
    }
    const std::size_t& Species::getID() const noexcept{
        return id;
    }
    void Species::setAge(const std::size_t& speciesAge) noexcept{
        age = speciesAge;
    }
    void Species::addAge(const std::size_t& amount) noexcept{
        age += amount;
    }
    const std::size_t& Species::getAge() const noexcept{
        return age;
    }
    bool Species::has(Genome* g) noexcept{
        auto found = std::find_if(std::begin(genomes),std::end(genomes),
                                [&](std::unique_ptr<Genome>& g2){
                                    return (g2.get() == g);
                                });
        return (found != std::end(genomes));
    }
    void Species::setGenomes(std::vector<std::unique_ptr<Genome>>&& gs) noexcept{
        genomes = std::move(gs);
    }
    void Species::addGenome(std::unique_ptr<Genome>&& g) noexcept{
        genomes.push_back(std::move(g));
    }
    void Species::removeGenome(Genome* g) noexcept{
        genomes.erase(std::remove_if(std::begin(genomes),std::end(genomes),
                        [&](std::unique_ptr<Genome>& g2){
                            return (g2.get() == g);
                        }), std::end(genomes));
    }
    std::vector<std::unique_ptr<Genome>>& Species::getGenomes() noexcept{
        return genomes;
    }
    const double& Species::getAvgFitness() const noexcept{
        return avgFitness;
    }
    const double& Species::getMaxFitness() const noexcept{
        return maxFitness;
    }
    std::size_t Species::getSize() const noexcept{
        return genomes.size();
    }
    bool Species::isStagnant() const noexcept{
        return (oldAvgFitness <= avgFitness);
    }
    JsonBox::Value Species::toJson() const noexcept{
        JsonBox::Object o;
        o["id"] = std::to_string(id);
        o["age"] = std::to_string(age);
        o["novel"] = novel;
        o["killable"] = killable;
        o["avgFitness"] = avgFitness;
        o["maxFitness"] = maxFitness;
        o["oldAvgFitness"] = oldAvgFitness;
        JsonBox::Array gnms;
        for(auto& g:genomes){
            gnms.push_back(g->toJson());
        }
        o["genomes"] = JsonBox::Value(gnms);
        return JsonBox::Value(o);
    }
    void Species::writeToFile(const std::string& filename) const noexcept{
        JsonBox::Value v;
        v["version"] = 1.0;
        v["Species"] = toJson();
        v.writeToFile(filename);
    }
    void Species::computeAvgFitness() noexcept{
        oldAvgFitness = avgFitness;
        auto sum = 0.0;
        for(auto& g:genomes){
            sum += g->getFitness();
        }
        avgFitness = sum / genomes.size();
    }
    void Species::computeMaxFitness() noexcept{
        auto max = 0.0;
        for(auto& g:genomes){
            if(g->getFitness() > max){
                max = g->getFitness();
            }
        }
        maxFitness = max;
    }
}