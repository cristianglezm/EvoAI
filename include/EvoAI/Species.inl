#include <algorithm>

namespace EvoAI{
    /**
     * @brief helper functions
     * @todo move to typeUtils?
     */
    namespace priv{
        template<typename T>
        std::remove_pointer_t<T>* to_pointer(std::remove_reference_t<std::remove_pointer_t<T>>& t) noexcept{
            return &t;
        }
        template<typename T>
        const std::remove_pointer_t<T>* to_pointer(const std::remove_reference_t<std::remove_pointer_t<T>>& t) noexcept{
            return &t;
        }
        template<typename T>
        std::remove_pointer_t<T>* to_pointer(std::remove_pointer_t<T>* t) noexcept{
            return t;
        }
        template<typename T>
        std::remove_pointer_t<T>* to_pointer(std::unique_ptr<T>& t) noexcept{
            return t.get();
        }
        template<typename T>
        std::remove_pointer_t<T>* to_pointer(std::shared_ptr<T>& t) noexcept{
            return t.get();
        }
        template<typename T>
        T* to_pointer(std::remove_reference_t<T>&& t) noexcept = delete;
        template<typename T>
        std::remove_reference_t<T>& to_ref(std::remove_reference_t<std::remove_pointer_t<T>>& t) noexcept{
            return &t;
        }
        template<typename T>
        const std::remove_reference_t<T>& to_ref(const std::remove_reference_t<std::remove_pointer_t<T>>& t) noexcept{
            return &t;
        }
        template<typename T>
        std::remove_reference_t<T>& to_ref(std::remove_pointer_t<T>* t) noexcept{
            return *t;
        }
        template<typename T>
        std::remove_reference_t<T>& to_ref(std::unique_ptr<T>& t) noexcept{
            return *t;
        }
        template<typename T>
        std::remove_reference_t<T>& to_ref(std::shared_ptr<T>& t) noexcept{
            return *t;
        }
        template<typename T>
        std::remove_reference_t<T>& to_ref(std::remove_reference_t<T>&& t) noexcept = delete;
    }
    template<typename T>
    Species<T>::Species() noexcept
    : members()
    , id(0)
    , age(0)
    , avgFitness(0.0)
    , maxFitness(0.0)
    , oldAvgFitness(0.0)
    , novel(true)
    , killable(true){}
    template<typename T>
    Species<T>::Species(const std::size_t& id, bool novel) noexcept
    : members()
    , id(id)
    , age(0)
    , avgFitness(0.0)
    , maxFitness(0.0)
    , oldAvgFitness(0.0)
    , novel(novel)
    , killable(true){}
    template<typename T>
    Species<T>::Species(JsonBox::Object o) noexcept
    : members()
    , id(std::stoull(o["id"].getString()))
    , age(std::stoull(o["age"].getString()))
    , avgFitness(o["avgFitness"].getDouble())
    , maxFitness(o["maxFitness"].getDouble())
    , oldAvgFitness(o["oldAvgFitness"].getDouble())
    , novel(o["novel"].getBoolean())
    , killable(o["killable"].getBoolean()){
        static_assert(!std::is_pointer_v<value_type>, "Error you can't call Species<T*>(JsonBox::Object) with T*");
        auto mmbs = o["members"].getArray();
        members.reserve(mmbs.size());
        for(auto& g:mmbs){
            members.emplace_back(g.getObject());
        }
    }
    template<typename T>
    Species<T>::Species(const std::string& filename)
    : members()
    , id(0)
    , age(0)
    , avgFitness(0.0)
    , maxFitness(0.0)
    , oldAvgFitness(0.0)
    , novel(true)
    , killable(true){
        static_assert(!std::is_pointer_v<value_type>, "Error you can't call Species<T*>(filename) with T*");
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
        auto mmbs = v["members"].getArray();
        members.reserve(mmbs.size());
        for(auto& g:mmbs){
            members.emplace_back(g.getObject());
        }
    }
    template<typename T>
    void Species<T>::adjustFitness() noexcept{
        auto size = members.size();
        if(size <= 1){
            return;
        }
        for(auto& m1:members){
            auto m1Ptr = priv::to_pointer<T>(m1);
            m1Ptr->setFitness(m1Ptr->getFitness() / size);
        }
    }
    template<typename T>
    void Species<T>::rank() noexcept{
        std::sort(std::begin(members),std::end(members),
                    [](auto& m1, auto& m2){
                        auto m1Ptr = priv::to_pointer<T>(m1);
                        auto m2Ptr = priv::to_pointer<T>(m2);
                        return (m1Ptr->getFitness() > m2Ptr->getFitness());
                    });
    }
    template<typename T>
    typename Species<T>::const_pointer Species<T>::getRepresentative() const noexcept{
        if constexpr(std::is_pointer_v<value_type>){
            if(!members.empty()){
                return members[0];
            }
        }else{
            if(!members.empty()){
                return &members[0];
            }
        }
        return nullptr;
    }
    template<typename T>
    typename Species<T>::pointer Species<T>::getChampion() noexcept{
        auto& champ = *std::max_element(std::begin(members), std::end(members), 
            [](auto& a, auto& b){
                auto aPtr = priv::to_pointer<T>(a);
                auto bPtr = priv::to_pointer<T>(b);
                return aPtr->getFitness() < bPtr->getFitness();
        });
        return priv::to_pointer<T>(champ);
    }
    template<typename T>
    bool Species<T>::isNovel() const noexcept{
        return novel;
    }
    template<typename T>
    void Species<T>::setNovel(bool n) noexcept{
        novel = n;
    }
    template<typename T>
    bool Species<T>::isKillable() const noexcept{
        return killable;
    }
    template<typename T>
    void Species<T>::setKillable(bool k) noexcept{
        killable = k;
    }
    template<typename T>
    void Species<T>::setID(const std::size_t& speciesID) noexcept{
        id = speciesID;
    }
    template<typename T>
    const std::size_t& Species<T>::getID() const noexcept{
        return id;
    }
    template<typename T>
    void Species<T>::setAge(const std::size_t& speciesAge) noexcept{
        age = speciesAge;
    }
    template<typename T>
    void Species<T>::addAge(const std::size_t& amount) noexcept{
        age += amount;
    }
    template<typename T>
    const std::size_t& Species<T>::getAge() const noexcept{
        return age;
    }
    template<typename T>
    void Species<T>::setMembers(std::vector<Species<T>::value_type>&& ms) noexcept{
        members = std::forward<T>(ms);
    }
    template<typename T>
    void Species<T>::add(std::conditional_t<std::is_pointer_v<Species<T>::value_type>, Species<T>::pointer, Species<T>::rvalue_reference> m) noexcept{
        members.emplace_back(std::forward<T>(m));
        if constexpr(std::is_pointer_v<T>){
            members.back()->setSpeciesID(id);
        }else{
            members.back().setSpeciesID(id);
        }
    }
    template<typename T>
    void Species<T>::remove(std::conditional_t<std::is_pointer_v<Species<T>::value_type>, Species<T>::pointer, std::size_t> id) noexcept{
        if constexpr(std::is_pointer_v<value_type>){
            if(id) members.erase(std::remove(std::begin(members), std::end(members), id), std::end(members));
        }else{
            members.erase(std::remove_if(std::begin(members),std::end(members),
                        [&id](auto& m){
                            return (m.getID() == id);
                        }), std::end(members));
        }
    }
    template<typename T>
    bool Species<T>::has(std::conditional_t<std::is_pointer_v<Species<T>::value_type>, Species<T>::pointer, std::size_t> id) noexcept{
        orderMembersByID();
        if constexpr(std::is_pointer_v<value_type>){
            if(id){
                auto comp = [](const auto& a, const auto& b) -> bool{
                    return a->getID() < b->getID();
                };
                auto start = std::lower_bound(std::begin(members), std::end(members), id, comp);
                return (!(start == std::end(members)) && !(comp(id, *start)));
            }
            return false;
        }else{
            auto comp1 = [](const auto& m, const auto& id) -> bool{
                return m.getID() < id;
            };
            auto comp2 = [](const auto& id, const auto& m) -> bool{
                return id < m.getID();
            };
            auto start = std::lower_bound(std::begin(members), std::end(members), id, comp1);
            return (!(start == std::end(members)) && !(comp2(id, *start)));
        }
    }
    template<typename T>
    void Species<T>::orderMembersByID() noexcept{
        if constexpr(std::is_pointer_v<value_type>){
            std::sort(std::begin(members),std::end(members),
                            [](auto& m1, auto& m2){
                                return (m1->getID() < m2->getID());
                        });
        }else{
            std::sort(std::begin(members),std::end(members),
                            [](auto& m1, auto& m2){
                                return (m1.getID() < m2.getID());
                        });
        }
    }
    template<typename T>
    std::vector<T>& Species<T>::getMembers() noexcept{
        return members;
    }
    template<typename T>
    double Species<T>::getAvgFitness() const noexcept{
        return avgFitness;
    }
    template<typename T>
    double Species<T>::getMaxFitness() const noexcept{
        return maxFitness;
    }
    template<typename T>
    std::size_t Species<T>::getSize() const noexcept{
        return members.size();
    }
    template<typename T>
    bool Species<T>::empty() const noexcept{
        return members.empty();
    }
    template<typename T>
    bool Species<T>::isStagnant() const noexcept{
        return (oldAvgFitness <= avgFitness);
    }
    template<typename T>
    JsonBox::Value Species<T>::toJson() const noexcept{
        JsonBox::Object o;
        o["id"] = std::to_string(id);
        o["age"] = std::to_string(age);
        o["novel"] = novel;
        o["killable"] = killable;
        o["avgFitness"] = avgFitness;
        o["maxFitness"] = maxFitness;
        o["oldAvgFitness"] = oldAvgFitness;
        JsonBox::Array mmbs;
        mmbs.reserve(members.size());
        for(const auto& g:members){
            auto gPtr = priv::to_pointer<T>(g);
            mmbs.emplace_back(gPtr->toJson());
        }
        o["members"] = JsonBox::Value(mmbs);
        return JsonBox::Value(o);
    }
    template<typename T>
    void Species<T>::writeToFile(const std::string& filename) const noexcept{
        JsonBox::Value v;
        v["version"] = 1.0;
        v["Species"] = toJson();
#if defined NDEBUG
        v.writeToFile(filename, false, false);
#else
        v.writeToFile(filename, true, false);
#endif
    }
    template<typename T>
    void Species<T>::computeAvgFitness() noexcept{
        oldAvgFitness = avgFitness;
        auto sum = std::accumulate(std::begin(members), std::end(members), 0.0d, 
            [](auto& a, auto& b) -> double{
                auto bPtr = priv::to_pointer<T>(b);
                return a + bPtr->getFitness();
        });
        auto size = members.size();
        avgFitness = sum / size ? size:1u;
    }
    template<typename T>
    void Species<T>::computeMaxFitness() noexcept{
        auto max = std::max_element(std::begin(members),std::end(members),
                        [](auto& a, auto& b){
                            auto aPtr = priv::to_pointer<T>(a);
                            auto bPtr = priv::to_pointer<T>(b);
                            return aPtr->getFitness() < bPtr->getFitness();
                    });
        if(max != std::end(members)){
            auto maxPtr = priv::to_pointer<T>(*max);
            maxFitness = maxPtr->getFitness();
        }
    }
}