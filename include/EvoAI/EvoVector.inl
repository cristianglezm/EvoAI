namespace EvoAI{
    template<typename T>
    EvoVector<T>::EvoVector(std::size_t size) noexcept
    : m_dna()
    , m_Id(0u)
    , m_speciesId(0u)
    , m_fitness(0.0){
        m_dna.reserve(size);
        for(auto i=0u;i<size;++i){
#ifdef __EMSCRIPTEN__
            if constexpr(std::is_same_v<T, char>){
                m_dna.emplace_back(static_cast<char>(randomGen().random<int>(-127, 127)));
            }else if constexpr(std::is_same_v<T, unsigned char>){
                m_dna.emplace_back(static_cast<unsigned char>(randomGen().random<int>(0, 255)));
            }else{
                m_dna.emplace_back(static_cast<T>(randomGen().random(std::numeric_limits<T>::min(), 
                                                                std::numeric_limits<T>::max())));
            }
#else
            m_dna.emplace_back(static_cast<T>(randomGen().random(std::numeric_limits<T>::min(), 
                                                            std::numeric_limits<T>::max())));
#endif
        }
    }
    template<typename T>
    template<typename Generator>
    EvoVector<T>::EvoVector(std::size_t size, Generator&& fn) noexcept
    : m_dna()
    , m_Id(0u)
    , m_speciesId(0u)
    , m_fitness(0.0){
        for(auto i=0u;i<size;++i){
            m_dna.emplace_back(fn());
        }
    }
    template<typename T>
    EvoVector<T>::EvoVector(DNA&& dna) noexcept
    : m_dna(std::forward<DNA>(dna))
    , m_Id(0u)
    , m_speciesId(0u)
    , m_fitness(0.0){}
    template<typename T>
    EvoVector<T>::EvoVector(JsonBox::Object o) noexcept
    : m_dna()
    , m_Id(std::stoul(o["Id"].tryGetString("0")))
    , m_speciesId(std::stoul(o["speciesId"].tryGetString("0")))
    , m_fitness(o["fitness"].tryGetDouble(0.0)){
        auto& ar = o["dna"].getArray();
        for(auto& d:ar){
            if constexpr(std::is_integral_v<T>){
                m_dna.emplace_back(static_cast<T>(d.getInteger()));
            }else if constexpr(std::is_floating_point_v<T>){
                m_dna.emplace_back(static_cast<T>(d.getDouble()));
            }
        }
    }
    template<typename T>
    JsonBox::Value EvoVector<T>::toJson() const noexcept{
        JsonBox::Object o;
        JsonBox::Array ar;
        ar.reserve(size());
        for(auto& c: m_dna){
            ar.emplace_back(c);
        }
        o["dna"] = ar;
        o["Id"] = std::to_string(m_Id);
        o["speciesID"] = std::to_string(m_Id);
        o["fitness"] = m_fitness;
        return o;
    }
    template<typename T>
    typename EvoVector<T>::value_type* EvoVector<T>::data() noexcept{
        return m_dna.data();
    }
	template<typename T>
	void EvoVector<T>::setID(std::size_t ID) noexcept{
		m_Id = ID;
	}
	template<typename T>
	std::size_t EvoVector<T>::getID() const noexcept{
		return m_Id;
	}
	template<typename T>
	void EvoVector<T>::setSpeciesID(std::size_t spcID) noexcept{
		m_speciesId = spcID;
	}
	template<typename T>
	void EvoVector<T>::setFitness(double fit) noexcept{
		m_fitness = fit;
	}
	template<typename T>
	void EvoVector<T>::addFitness(double amount) noexcept{
		m_fitness += amount;
	}
	template<typename T>
	double EvoVector<T>::getFitness() const noexcept{
		return m_fitness;
	}
	template<typename T>
	std::size_t EvoVector<T>::getSpeciesID() const noexcept{
		return m_speciesId;
	}
	template<typename T>
	std::size_t EvoVector<T>::size() const noexcept{
		return m_dna.size();
	}
	template<typename T>
	typename EvoVector<T>::reference EvoVector<T>::operator[](int index) noexcept{
		return m_dna[index];
	}
	template<typename T>
	typename EvoVector<T>::const_reference EvoVector<T>::operator[](int index) const noexcept{
		return m_dna[index];
	}
	template<typename T>
	typename EvoVector<T>::iterator EvoVector<T>::begin() noexcept{
		return m_dna.begin();
	}
	template<typename T>
	typename EvoVector<T>::iterator EvoVector<T>::end() noexcept{
		return m_dna.end();
	}
    template<typename T>
	typename EvoVector<T>::const_iterator EvoVector<T>::begin() const noexcept{
        return m_dna.begin();
    }
    template<typename T>
	typename EvoVector<T>::const_iterator EvoVector<T>::end() const noexcept{
        return m_dna.end();
    }
    template<typename T>
    void EvoVector<T>::mutate(float rate) noexcept{
        std::size_t index = randomGen().random(std::size_t(0), size() - 1);
        if(randomGen().random(rate)){
            if(m_dna[index] < std::numeric_limits<T>::max()){
                m_dna[index] = ++m_dna[index];
            }
        }else{
            if(m_dna[index] > std::numeric_limits<T>::min()){
                m_dna[index] = --m_dna[index];
            }
        }
    }
// static fn
    template<typename T>
    EvoVector<T> EvoVector<T>::reproduce(const EvoVector<T>& es1, const EvoVector<T>& es2) noexcept{
        if(es1.size() != es2.size()){
            return es1;
        }
        EvoVector<T>::DNA dna;
        for(auto i=0u;i<es1.size();++i){
            if(randomGen().random(0.5)){
                dna.emplace_back(es1.m_dna[i]);
            }else{
                dna.emplace_back(es2.m_dna[i]);
            }
        }
        return EvoVector<T>(std::move(dna));
    }
    template<typename T>
    double EvoVector<T>::distance(const EvoVector& es1, const EvoVector& es2, 
                [[maybe_unused]] double c1, 
                [[maybe_unused]] double c2, 
                [[maybe_unused]] double c3) noexcept{
        if(es1.size() != es2.size()){
            return 1000.0;
        }
        auto sum = 0.0;
        for(auto i=0u;i<es1.size();++i){
            sum += es1.m_dna[i] != es2.m_dna[i];
        }
        return sum / es1.size();
    }
}
