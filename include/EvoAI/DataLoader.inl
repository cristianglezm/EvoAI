namespace EvoAI{
    template<class Dataset>
    DataLoader<Dataset>::DataLoader(Dataset&& ds, bool randomize)
    : m_randomize(randomize)
    , m_ds(ds){}
    template<class Dataset>
    std::pair<std::vector<double>&, std::vector<double>&> DataLoader<Dataset>::operator()() noexcept{
        return m_ds();
    }
    template<class Dataset>
    std::size_t DataLoader<Dataset>::size() noexcept{
        return m_ds.size();
    }
    template<class Dataset>
    std::size_t DataLoader<Dataset>::getBatchSize() noexcept{
        return m_ds.getBatchSize();
    }
    template<class Dataset>
    void DataLoader<Dataset>::shuffle() noexcept{
        if(m_randomize){
            m_ds.shuffle();
        }
    }
    template<class Dataset>
    Dataset& DataLoader<Dataset>::getDataset() noexcept{
        return m_ds;
    }
}