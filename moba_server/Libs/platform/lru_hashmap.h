#pragma once
/*
 * 用来记录冷热数据,内存中前面为热数据(在线玩家)
 * 有玩家人数限制
 */

#include <list>
#include <utility>
#include <functional>
#include <unordered_map>

template <typename KeyT, typename ValT> struct lru_hashmap_list_t
{
    typedef std::pair<KeyT, ValT>                   value_type;
    typedef std::list<value_type>                   list_type;
    typedef typename list_type::iterator            node_type;
    typedef typename list_type::iterator            iterator;
    typedef typename list_type::const_iterator      const_iterator;
};
;

template<typename KeyT, typename ValT>
class LogicLRUHashmap
{
public:
    enum DEFAULT_PARAM
    {
        DEFAULT_INIT_HASHTABLE_SIZE = 5000
    };

public:
    typedef ValT data_type;
    typedef ValT mapped_type;
    typedef typename lru_hashmap_list_t<KeyT, ValT>::list_type list_type;               //list类型
    typedef typename lru_hashmap_list_t<KeyT, ValT>::node_type node_type;               //list::iterator类型
    typedef typename lru_hashmap_list_t<KeyT, ValT>::iterator iterator;                 //list::iterator类型
    typedef typename lru_hashmap_list_t<KeyT, ValT>::const_iterator const_iterator;     //list::const_iterator类型
    typedef typename lru_hashmap_list_t<KeyT, ValT>::value_type value_type;             //list成员类型
    typedef std::unordered_map<KeyT, node_type> hash_table_type;                        //hashtable类型
    typedef typename hash_table_type::key_type key_type;                                //hashtable::key类型
    typedef typename hash_table_type::value_type internal_value_type;                   //hashtable::value类型
    typedef typename hash_table_type::iterator internal_iterator;                       //hashtable::iterator类型
    typedef typename hash_table_type::const_iterator const_internal_iterator;           //hashtable::const_iterator类型

    typedef typename hash_table_type::size_type size_type;

    typedef typename hash_table_type::allocator_type allocator_type;

    typedef std::function<void(const KeyT&, const ValT&)> notifier_callback_type;

public:
    LogicLRUHashmap()
        : m_stLRUList(), 
        m_stHashTable((size_type)DEFAULT_INIT_HASHTABLE_SIZE), 
        m_stNotifier(), 
        m_uiMaxNodeNum((size_type)DEFAULT_INIT_HASHTABLE_SIZE) {}

    explicit LogicLRUHashmap(size_type uiInitSize)
        : m_stLRUList(), 
        m_stHashTable(uiInitSize),
        m_stNotifier(), 
        m_uiMaxNodeNum(uiInitSize) {}

    //O(1)
    size_type Size() const { return (m_stHashTable.size()); }

    bool Empty() const { return (m_stLRUList.empty()); }

    iterator Begin() { return (m_stLRUList.begin()); }

    iterator End() { return (m_stLRUList.end()); }

    const_iterator Begin() const { return (m_stLRUList.begin()); }

    const_iterator End() const { return (m_stLRUList.end()); }

    void SetKickOutNotifier(const notifier_callback_type& stNotifier)
    {
        m_stNotifier = stNotifier;
    }

    std::pair<iterator, bool> Insert(const value_type& stObj)
    {
        if (m_stHashTable.find(stObj.first) != m_stHashTable.end())
        {
            return (std::pair<iterator, bool>(m_stLRUList.end(), false));
        }

        //超出限制最大数,去除冷数据(队列中最后一个)
        if (m_stHashTable.size() >= m_uiMaxNodeNum) DelOldestElem();

        node_type stNode = AddNodeToNewest(stObj);

        m_stHashTable.emplace(stObj.first, stNode);

        return (std::pair<iterator, bool>(stNode, true));
    }

    iterator Find(const key_type& stKey)
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if (stIter != m_stHashTable.end())
        {
            return (UpdateElemToNewest(stIter->second));
        }

        return (m_stLRUList.end());
    }

    const_iterator Find(const key_type& stKey) const
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if (stIter != m_stHashTable.end())
        {
            return (const_iterator(UpdateElemToNewest(stIter->second)));
        }

        return (m_stLRUList.end());
    }

    iterator FindNoMove(const key_type& stKey)
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if (stIter != m_stHashTable.end()) return (stIter->second);

        return (m_stLRUList.end());
    }

    const_iterator FindNoMove(const key_type& stKey) const
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if (stIter != m_stHashTable.end()) return (const_iterator(stIter->second));

        return (m_stLRUList.end());
    }

    ValT& operator[](const key_type& stKey)
    {
        internal_iterator stIter = m_stHashTable.find(stKey);
        if (stIter != m_stHashTable.end())
        {
            return (UpdateElemToNewest(stIter->second)->second);
        }
        else
        {
            std::pair<iterator, bool> stRet = Insert(value_type(stKey, ValT()));
            return (stRet->first->second);
        }
    }

    size_type Erase(const key_type& stKey)
    {
        internal_iterator stInternalIter = m_stHashTable.find(stKey);
        if (stInternalIter != m_stHashTable.end())
        {
            DelElem(stInternalIter->second);
            m_stHashTable.erase(stInternalIter);
        }

        return (m_stHashTable.size());
    }

    void Erase(iterator stIter)
    {
        if (End() != stIter)
        {
            m_stHashTable.erase(stIter->first);
            DelElem(stIter);
        }
    }

    void Clear()
    {
        m_stHashTable.clear();
        m_stLRUList.clear();
    }

private:
    bool IsNotifierValid() { return ((bool)m_stNotifier); }

    void NotifyKickout(const value_type& stVal) { if (IsNotifierValid()) m_stNotifier(stVal.first, stVal.second); }

    node_type AddNodeToNewest(const value_type& stVal)
    {
        m_stLRUList.push_front(stVal);
        return (m_stLRUList.begin());
    }

    node_type UpdateElemToNewest(node_type& stNode)
    {
        value_type stValue(*stNode);
        m_stLRUList.erase(stNode);
        m_stLRUList.push_front(stValue);
        stNode = m_stLRUList.begin();
        return (stNode);
    }

    void DelOldestElem()
    {
        value_type stVal = m_stLRUList.back();
        m_stHashTable.erase(stVal.first);
        NotifyKickout(stVal);
        m_stLRUList.pop_back();
    }

    void DelElem(node_type& stNode) { m_stLRUList.erase(stNode); }

private:
    list_type                       m_stLRUList;
    mutable hash_table_type         m_stHashTable;
    notifier_callback_type          m_stNotifier;

    size_type                       m_uiMaxNodeNum;
};




