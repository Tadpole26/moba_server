#pragma once

#include <map>

 //value只能用指针存储,value的指针的创建和释放由容器托管
template <typename KeyT, typename TableT, typename TableValT>
class RecordAssociateContainer {  };

template <typename KeyT, typename TableT, typename TableValT>
class RecordAssociateContainer<KeyT, TableT*, TableValT>
{
public:
    typedef std::map<KeyT, TableT*>                         map_container_type;
    typedef typename map_container_type::iterator           iterator;
    typedef typename map_container_type::const_iterator     const_iterator;
    typedef typename map_container_type::mapped_type        mapped_type;
    typedef typename map_container_type::value_type         value_type;
    typedef typename map_container_type::key_type           key_type;
    typedef typename map_container_type::size_type          size_type;
    typedef TableValT                                       table_mapped_type;
    typedef std::pair<KeyT, table_mapped_type>              table_value_type;

public:
    RecordAssociateContainer() : m_stRecordMap() {}

    ~RecordAssociateContainer()
    {
        for (auto stIter = m_stRecordMap.begin(); stIter != m_stRecordMap.end(); ++stIter)
        {
            delete stIter->second;
        }
    }

    RecordAssociateContainer(const RecordAssociateContainer&) = delete;
    RecordAssociateContainer& operator=(const RecordAssociateContainer&) = delete;

    iterator Begin() { return (m_stRecordMap.begin()); }
    const_iterator Begin() const { return (m_stRecordMap.begin()); }
    iterator End() { return (m_stRecordMap.end()); }
    const_iterator End() const { return (m_stRecordMap.end()); }

    bool Empty() const { return (m_stRecordMap.empty()); }
    size_type Size() const { return (m_stRecordMap.size()); }

    iterator Find(const key_type& stKey) { return (m_stRecordMap.find(stKey)); }
    const_iterator Find(const key_type& stKey) const { return (m_stRecordMap.find(stKey)); }

    std::pair<iterator, bool> Insert(const table_value_type& stVal)
    {
        std::pair<iterator, bool> stRet;
        stRet.first = m_stRecordMap.end();
        stRet.second = false;
        if (m_stRecordMap.find(stVal.first) == m_stRecordMap.end())
        {
            TableT* pTableMappedValue = new TableT();
            pTableMappedValue->Initialize(stVal.second);
            stRet = m_stRecordMap.insert(value_type(stVal.first, pTableMappedValue));
        }

        return (stRet);
    }

    std::pair<iterator, bool> InsertWithoutSQL(const table_value_type& stVal)
    {
        std::pair<iterator, bool> stRet;
        stRet.first = m_stRecordMap.end();
        stRet.second = false;
        if (m_stRecordMap.find(stVal.first) == m_stRecordMap.end())
        {
            TableT* pTableMappedValue = new TableT();
            pTableMappedValue->InitializeWithoutSQL(stVal.second);
            stRet = m_stRecordMap.insert(value_type(stVal.first, pTableMappedValue));
        }

        return (stRet);
    }

    void Erase(iterator stIter)
    {
        if (stIter != m_stRecordMap.end())
        {
            TableT* pTableMappedValue = stIter->second;
            m_stRecordMap.erase(stIter);
            pTableMappedValue->ClearFromSQL();

            delete pTableMappedValue;
        }
    }

    size_type Erase(const KeyT& stKey)
    {
        Erase(m_stRecordMap.find(stKey));

        return (m_stRecordMap.size());
    }

private:
    std::map<KeyT, TableT*>         m_stRecordMap;
};




