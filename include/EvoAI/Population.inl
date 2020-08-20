namespace EvoAI{
    template<typename T>
    Population<T>::Population()
    : species()
    , members()
    , PopulationSize(50)
    , maxAge(120)
    , speciesID(0u)
    , memberID(0u)
    , compatibilityThreshold(2.0)
    , membersCached(false){}
    template<typename T>
    Population<T>::Population(std::function<T()>&& fn, std::size_t size)
    : species()
    , members()
    , PopulationSize(size)
    , maxAge(120)
    , speciesID(0u)
    , memberID(0u)
    , compatibilityThreshold(2.0)
    , membersCached(false){
        for(auto i=0u;i<size;++i){
            addMember(fn());
        }
    }
    template<typename T>
    template<typename...Args>
    Population<T>::Population(std::size_t size, Args...args)
    : species()
    , members()
    , PopulationSize(size)
    , maxAge(120)
    , speciesID(0u)
    , memberID(0u)
    , compatibilityThreshold(2.0)
    , membersCached(false){
        static_assert(!std::is_pointer_v<T>, "Population<T*>(std::size_t size, Args...args) cannot be used, use Population<T*>(std::function<T()>&& fn, std::size_t size) instead.");
        members.reserve(size);
        for(auto i=0u;i<size;++i){
            addMember(T(args...));
        }
    }
    template<typename T>
    Population<T>::Population(JsonBox::Object o)
    : species()
    , members()
    , PopulationSize(std::stoull(o["PopulationSize"].getString()))
    , speciesID(0u)
    , memberID(0u)
    , maxAge(std::stoull(o["maxAge"].getString()))
    , compatibilityThreshold(o["compatibilityThreshold"].getDouble())
    , membersCached(false){
        static_assert(!std::is_pointer_v<T>, "Population<T*>(JsonBox::Object o) cannot be used to load data.");
        auto& specs = o["species"].getArray();
        members.reserve(PopulationSize);
        for(auto& sp:specs){
            auto spPtr = std::make_unique<Species<T>>(sp.getObject());
            auto id = spPtr->getID();
            species.emplace(id, std::move(spPtr));
        }
    }
    template<typename T>
    Population<T>::Population(const std::string& filename)
    : species()
    , members()
    , PopulationSize(50)
    , maxAge(120)
    , speciesID(0u)
    , memberID(0u)
    , compatibilityThreshold(2.0)
    , membersCached(false){
        static_assert(!std::is_pointer_v<T>, "Population<T*> cannot be used to load data.");
        JsonBox::Value json;
        json.loadFromFile(filename);
        auto& v = json["Population"];
        auto& specs = v["species"].getArray();
        members.reserve(PopulationSize);
        for(auto& sp:specs){
            auto spPtr = std::make_unique<Species<T>>(sp.getObject());
            auto id = spPtr->getID();
            species.emplace(id, std::move(spPtr));
        }
        PopulationSize = std::stoull(v["PopulationSize"].getString());
        compatibilityThreshold = v["compatibilityThreshold"].getDouble();
        maxAge = std::stoull(v["maxAge"].getString());
    }
    template<typename T>
    void Population<T>::addMember(std::conditional_t<std::is_pointer_v<Population<T>::value_type>, Population<T>::pointer, Population<T>::rvalue_reference> m, double c1, double c2, double c3) noexcept{
        membersCached = false;
        if constexpr(std::is_pointer_v<T>){
#ifndef NDEBUG
            assert(m != nullptr && "m cannot be nullptr");
#endif
            for(auto& [id, sp]:species){
                auto rep = sp->getRepresentative();
                if(rep){
                    auto dist = std::remove_pointer_t<pointer>::distance(*m,*rep, c1, c2, c3);
                    if(dist >= -compatibilityThreshold && dist <= compatibilityThreshold){
                        sp->add(m);
                        return;
                    }
                }
            }
            auto sp = std::make_unique<Species<T>>(getNewSpeciesID(), true);
            sp->add(m);
            auto id = sp->getID();
            species.emplace(id, std::move(sp));
        }else{
            m.setID(Population::getNewMemberID());
            for(auto& [id, sp]:species){
                auto rep = sp->getRepresentative();
                if(rep){
                    auto dist = T::distance(m,*rep, c1, c2, c3);
                    if(dist >= -compatibilityThreshold && dist <= compatibilityThreshold){
                        sp->add(std::move(m));
                        return;
                    }
                }
            }
            auto sp = std::make_unique<Species<T>>(getNewSpeciesID(), true);
            sp->add(std::move(m));
            species.emplace(sp->getID(), std::move(sp));
        }
    }
    template<typename T>
    void Population<T>::removeMember(Population<T>::reference m) noexcept{
        auto sp = findSpecies(m.getSpeciesID());
        if constexpr(std::is_pointer_v<T>){
            if(sp){
                sp->remove(&m);
                // we can't have an empty species because there won't be any representative for it.
                if(sp->empty()){
                    removeSpecies(*sp);
                }
            }
        }else{
            if(sp){
                sp->remove(m.getID());
                // we can't have an empty species because there won't be any representative for it.
                if(sp->empty()){
                    removeSpecies(*sp);
                }
            }
        }
        membersCached = false;
    }
    template<typename T>
    void Population<T>::removeMembers(std::vector<Population<T>::pointer>&& membrs) noexcept{
        if constexpr(std::is_pointer_v<T>){
            for(auto m:membrs){
                removeMember(*m);
            }
        }else{
            std::vector<std::pair<std::size_t, std::size_t>> ids;
            ids.reserve(membrs.size());
            for(auto m:membrs){
                ids.emplace_back(m->getSpeciesID(), m->getID());
            }
            for(auto [spID, mID]: ids){
                auto sp = findSpecies(spID);
                if(sp){
                    sp->remove(mID);
                    if(sp->empty()){
                        removeSpecies(*sp);
                    }
                }
            }
        }
        membersCached = false;
    }
    template<typename T>
    void Population<T>::addSpecies(std::unique_ptr<Species<T>>&& sp) noexcept{
        auto id = sp->getID();
#ifndef NDEBUG
        assert(!sp->empty() && "Species must not be empty.");
#endif
        species.insert_or_assign(id, std::move(sp));
        membersCached = false;
    }
    template<typename T>
    void Population<T>::removeSpecies(const Species<T>& sp) noexcept{
        species.erase(sp.getID());
        membersCached = false;
    }
    template<typename T>
    void Population<T>::eval(std::function<void(typename Population<T>::reference)>&& fn) noexcept{
        for(auto& m:getMembers()){
            fn(*m);
        }
    }
    template<typename T>
    std::vector<typename Population<T>::pointer>& Population<T>::getMembers() noexcept{
        if(membersCached){
            return members;
        }
        membersCached = true;
        members.clear();
        members.reserve(PopulationSize);
        for(auto& [id, sp]:species){
            for(auto& m:sp->getMembers()){
                if constexpr(std::is_pointer_v<value_type>){
                    members.emplace_back(m);
                }else{
                    members.emplace_back(&m);
                }
            }
        }
        return members;
    }
    template<typename T>
    typename Population<T>::pointer Population<T>::findMember(std::size_t id) noexcept{
        auto& mems = getMembers();
        auto found = std::find_if(std::begin(mems),std::end(mems),
                                    [id](pointer m){
                                        return (m->getID() == id);
                                    });
        if(found != std::end(mems)){
            return *found;
        }
        return nullptr;
    }
    template<typename T>
    Species<T>* Population<T>::findSpecies(std::size_t id) noexcept{
        auto found = species.find(id);
        if(found != std::end(species)){
            return found->second.get();
        }
        return nullptr;
    }
    template<typename T>
    typename Population<T>::pointer Population<T>::getBestMember() noexcept{
        auto& mems = getMembers();
        auto best = std::max_element(std::begin(mems), std::end(mems), 
            [](auto a, auto b){
                return a->getFitness() < b->getFitness(); ///@todo review
        });
        if(best != std::end(mems)){
            return *best;
        }
        return nullptr;
    }
    template<typename T>
    typename Population<T>::species_map& Population<T>::getSpecies() noexcept{
        return species;
    }
    template<typename T>
    typename Population<T>::species_ids_or_void_t Population<T>::removeOldSpecies() noexcept{
        auto iter = species.begin();
        membersCached = false;
        if constexpr(std::is_pointer_v<T>){
            std::vector<std::size_t> ids;
            while(iter != species.end()){
                auto sp = iter->second.get();
                if(((sp->getAge() > maxAge) || sp->empty()) && sp->isKillable()){
                    ids.emplace_back(iter->first);
                    iter = species.erase(iter);
                }else{
                    ++iter;
                }
            }
            return ids;
        }else{
            while(iter != species.end()){
                auto sp = iter->second.get();
                if(((sp->getAge() > maxAge) || sp->empty()) && sp->isKillable()){
                    iter = species.erase(iter);
                }else{
                    ++iter;
                }
            }
        }
    }
    template<typename T>
    void Population<T>::increaseAge() noexcept{
        auto iter = species.begin();
        while(iter != species.end()){
            auto sp = iter->second.get();
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
            ++iter;
        }
    }
    template<typename T>
    typename Population<T>::species_ids_or_void_t Population<T>::increaseAgeAndRemoveOldSpecies() noexcept{
        auto iter = species.begin();
        membersCached = false;
        if constexpr(std::is_pointer_v<T>){
            std::vector<std::size_t> ids;
            while(iter != species.end()){
                auto sp = iter->second.get();
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
                if(((sp->getAge() > maxAge) || sp->empty()) && sp->isKillable()){
                    ids.emplace_back(iter->first);
                    iter = species.erase(iter);
                }else{
                    ++iter;
                }
            }
            return ids;
        }else{
            while(iter != species.end()){
                auto sp = iter->second.get();
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
                if(((sp->getAge() > maxAge) || sp->empty()) && sp->isKillable()){
                    iter = species.erase(iter);
                }else{
                    ++iter;
                }
            }
        }
    }
    template<typename T>
    template<typename...Args>
    void Population<T>::regrowPopulation(Args...args) noexcept{
        static_assert(!std::is_pointer_v<T>, "Population<T*>::regrowPopulation(Args...) cannot be used, use Population<T*>::regrowPopulation(Fn) instead.");
        if(getPopulationMaxSize() <= getPopulationSize()){
            return;
        }
        auto numOffsprings = std::floor((getPopulationMaxSize() - getPopulationSize()));
        for(auto i=0u;i<numOffsprings;++i){
            addMember(T(args...));
        }
    }
    template<typename T>
    template<typename Fn>
    void Population<T>::regrowPopulation(Fn&& fn) noexcept{
        if(getPopulationMaxSize() <= getPopulationSize()){
            return;
        }
        auto numOffsprings = getPopulationMaxSize() - getPopulationSize();
        for(auto i=0u;i<numOffsprings;++i){
            addMember(fn());
        }
    }
    template<typename T>
    template<typename SelectionAlgo>
    typename Population<T>::result_or_void_t Population<T>::regrowPopulationFromElites(SelectionAlgo&& sa, bool interSpecies) noexcept{
        if(getPopulationMaxSize() <= getPopulationSize()){
            if constexpr(std::is_pointer_v<T>){
                return make_result();
            }else{
                return;
            }
        }
        membersCached = false; // make sure the cache is rebuilt
        auto numOffsprings = getPopulationMaxSize() - getPopulationSize();
        if constexpr(std::is_pointer_v<T>){
            std::vector<SelectionAlgorithms::Selected<T>> selected;
            auto result = make_result();
            if(interSpecies){
                selected = sa(getMembers(), numOffsprings);
            }else{
                selected = sa(species, numOffsprings);
            }
            for(auto& sel:selected){
                result.second.emplace_back(std::remove_pointer_t<T>::reproduce(*sel.father, *sel.mother));
            }
            membersCached = false; // the cache should be invalidated again.
            return result;
        }else{
            std::vector<SelectionAlgorithms::Selected<T>> selected;
            if(interSpecies){
                selected = sa(getMembers(), numOffsprings);
            }else{
                selected = sa(species, numOffsprings);
            }
            std::vector<T> kids;
            kids.reserve(selected.size());
            // addMember will invalidate selected pointers.
            for(auto& sel:selected){
                kids.emplace_back(T::reproduce(*sel.father, *sel.mother));
            }
            for(auto& k:kids){
                addMember(std::move(k));
            }
        }
    }
    template<typename T>
    template<typename SelectionAlgo>
    typename Population<T>::result_or_void_t Population<T>::reproduce(SelectionAlgo&& sa, bool interSpecies) noexcept{
        std::size_t numToSelect = std::floor(getPopulationSize() / 2);
        membersCached = false; // make sure the cache is rebuilt
        if constexpr(std::is_pointer_v<T>){
            std::vector<SelectionAlgorithms::Selected<T>> selected;
            auto result = make_result();
            if(interSpecies){
                selected = sa(getMembers(), numToSelect);
            }else{
                selected = sa(species, numToSelect);
            }
            auto size = selected.size();
            result.first.reserve(size);
            result.second.reserve(size);
            for(auto& sel:selected){
                result.first.emplace_back(sel.loser);
                result.second.emplace_back(std::remove_pointer_t<T>::reproduce(*sel.father, *sel.mother));
            }
            membersCached = false; // the cache should be invalidated again.
            return result;
        }else{
            std::vector<SelectionAlgorithms::Selected<T>> selected;
            if(interSpecies){
                selected = sa(getMembers(), numToSelect);
            }else{
                selected = sa(species, numToSelect);
            }
            std::vector<T> kids;
            kids.reserve(selected.size());
            // addMember will invalidate selected pointers.
            for(auto& sel:selected){
                kids.emplace_back(T::reproduce(*sel.father, *sel.mother));
            }
            std::vector<pointer> losers;
            losers.reserve(selected.size());
            for(auto& sel:selected){
                losers.push_back(sel.loser);
            }
            removeMembers(std::move(losers));
            for(auto& k:kids){
                addMember(std::move(k));
            }
        }
        membersCached = false;
    }
    template<typename T>
    void Population<T>::orderMembersByFitness() noexcept{
        getMembers();
        std::sort(std::begin(members),std::end(members),
                        [](pointer g1, pointer g2){
                            return (g1->getFitness() > g2->getFitness());
                    });
    }
    template<typename T>
    void Population<T>::orderMembersByID() noexcept{
        getMembers();
        std::sort(std::begin(members),std::end(members),
                        [](pointer g1, pointer g2){
                            return (g1->getID() < g2->getID());
                    });
    }
    template<typename T>
    void Population<T>::rankWithinSpecies() noexcept{
        for(auto& [id, sp]:species){
            sp->rank();
        }
        membersCached = false;
    }
    template<typename T>
    void Population<T>::setPopulationMaxSize(std::size_t maxSize) noexcept{
        PopulationSize = maxSize;
    }
    template<typename T>
    std::size_t Population<T>::getPopulationMaxSize() const noexcept{
        return PopulationSize;
    }
    template<typename T>
    std::size_t Population<T>::getPopulationSize() noexcept{
        return std::accumulate(std::begin(species),std::end(species),0,
            [](auto& a, auto& b){
                return a + b.second->getSize();
        });
    }
    template<typename T>
    std::size_t Population<T>::getSpeciesSize() const noexcept{
        return species.size();
    }
    template<typename T>
    void Population<T>::clear() noexcept{
        speciesID = 0;
        memberID = 0;
        membersCached = false;
        species.clear();
    }
    template<typename T>
    void Population<T>::setMaxAge(std::size_t age) noexcept{
        maxAge = age;
    }
    template<typename T>
    const std::size_t& Population<T>::getMaxAge() const noexcept{
        return maxAge;
    }
    template<typename T>
    void Population<T>::setCompatibilityThreshold(double compThreshold) noexcept{
        compatibilityThreshold = compThreshold;
    }
    template<typename T>
    double Population<T>::getCompatibilityThreshold() const noexcept{
        return compatibilityThreshold;
    }
    template<typename T>
    double Population<T>::computeAvgFitness() noexcept{
        auto& membs = getMembers();
        double sumFitness = std::accumulate(std::begin(membs), std::end(membs), 0.0d,
            [](auto& a, auto& b){
                return a + b->getFitness();
        });
        auto size = membs.size();
        return (sumFitness / (size ? size:1));
    }
    template<typename T>
    JsonBox::Value Population<T>::toJson() const noexcept{
        JsonBox::Object o;
        JsonBox::Array spcs;
        spcs.reserve(species.size());
        for(auto& [id, sp]:species){
            spcs.emplace_back(sp->toJson());
        }
        o["species"] = JsonBox::Value(spcs);
        o["PopulationSize"] = std::to_string(PopulationSize);
        o["compatibilityThreshold"] = compatibilityThreshold;
        o["maxAge"] = std::to_string(maxAge);
        return JsonBox::Value(o);
    }
    template<typename T>
    void Population<T>::writeToFile(const std::string& filename){
        JsonBox::Value v;
        v["version"] = 1.0;
        v["Population"] = toJson();
#if defined NDEBUG
        v.writeToFile(filename, false, false);
#else
        v.writeToFile(filename, true, false);
#endif
    }
//////////////
///// private
//////////////
    template<typename T>
    std::size_t Population<T>::getNewSpeciesID() noexcept{
        return speciesID++;
    }
    template<typename T>
    std::size_t Population<T>::getNewMemberID() noexcept{
        return memberID++;
    }
}