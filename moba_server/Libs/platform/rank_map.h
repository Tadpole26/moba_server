#pragma once
//排行榜模板类

#include <tuple>
#include <functional>
#include <unordered_map>
#include "skiplist.h"

template <class KeyT, class ValT, class ComparatorT = std::greater<int64_t>>
class RankMap
{
public:
    const static unsigned int DEFAULT_MAX_RANK = 5000;

public:
    typedef int64_t												    score_type;
    typedef std::pair<KeyT, ValT>                                   data_type;
    typedef std::pair<data_type, score_type>                        value_type;

    struct comparator {
        bool operator()(const value_type& x, const value_type& y) const {
            return ComparatorT()(x.second, y.second);
        }
    };

    typedef CTCSkipList<value_type, comparator>                     skip_list_type;
    typedef typename skip_list_type::iterator                       iterator;
    typedef typename skip_list_type::const_iterator                 const_iterator;
    typedef typename skip_list_type::reverse_iterator               reverse_iterator;
    typedef typename skip_list_type::const_reverse_iterator         const_reverse_iterator;
    typedef std::unordered_map<KeyT, iterator>                      hash_map_type;
    typedef typename hash_map_type::size_type                       size_type;

public:
    RankMap() : m_uiMaxRank(DEFAULT_MAX_RANK), m_stRankList(), m_stKeyHash(m_uiMaxRank) {};

    RankMap(unsigned int uiMaxRank) : m_uiMaxRank(uiMaxRank), m_stRankList(), m_stKeyHash(m_uiMaxRank)
    {
        if (uiMaxRank < 16) uiMaxRank = 16;
    };

    /**
     * 返回值:第一个值表示插入后的排名,0表示没有排名第二个值为true表示该用户的不存在表中,false反之。
     */
    std::pair<unsigned int, bool> Insert(const data_type& stVal, score_type uiScore)
    {
        typename hash_map_type::iterator stIT = m_stKeyHash.find(stVal.first);
        if (stIT == m_stKeyHash.end())
        {
            if ((m_stRankList.Size() >= m_uiMaxRank) && (m_stRankList.Empty() == false))
            {
                if (uiScore <= m_stRankList.REnd()->second)
                {
                    return (std::pair<unsigned int, bool>(0, true));
                }
            }

            std::pair<unsigned int, iterator> stRet = m_stRankList.Insert(std::pair<data_type, score_type>(stVal, uiScore));
            m_stKeyHash[stVal.first] = stRet.second;
            if (m_stRankList.Size() > m_uiMaxRank)
            {
                iterator stMinScoreIT = m_stRankList.End();
                --stMinScoreIT;

                m_stKeyHash.erase(stMinScoreIT->first.first);
                m_stRankList.Erase(stMinScoreIT);
            }

            return (std::pair<unsigned int, bool>(stRet.first, true));
        }

        return (std::pair<unsigned int, bool>(0, false));
    }

    /**
     * 返回值:第一个值表示修改前的排名,第二个值表示修改后的排名。0:表示没有排名
     */
    std::pair<unsigned int, unsigned int> UpdateOrInsert(const data_type& stVal, score_type uiScore)
    {
        typename hash_map_type::iterator stIT = m_stKeyHash.find(stVal.first);
        if (stIT == m_stKeyHash.end())
        {
            if (m_stRankList.Size() >= m_uiMaxRank && m_stRankList.Empty() == false)
            {
                if (ComparatorT()(m_stRankList.RBegin()->second, uiScore))
                {
                    return (std::pair<unsigned int, unsigned int>(0, 0));
                }
            }

            std::pair<unsigned int, iterator> stRet = m_stRankList.Insert(std::pair<data_type, score_type>(stVal, uiScore));
            m_stKeyHash[stVal.first] = stRet.second;
            if (m_stRankList.Size() > m_uiMaxRank)
            {
                iterator stMinScoreIT = m_stRankList.End();
                --stMinScoreIT;

                m_stKeyHash.erase(stMinScoreIT->first.first);
                m_stRankList.Erase(stMinScoreIT);
            }

            return (std::pair<unsigned int, unsigned int>(0, stRet.first));
        }
        else
        {
            unsigned int uiPrevRank = m_stRankList.GetRank(stIT->second);
            if (uiScore == stIT->second->second)
            {
                return (std::pair<unsigned int, unsigned int>(uiPrevRank, uiPrevRank));
            }
            else
            {
                m_stRankList.Erase(stIT->second);
                std::pair<unsigned int, iterator> stRet = m_stRankList.Insert(std::pair<data_type, score_type>(stVal, uiScore));
                stIT->second = stRet.second;

                return (std::pair<unsigned int, unsigned int>(uiPrevRank, stRet.first));
            }
        }
    }

    /**
     * 返回值:名次，0表示没有名次
     */
    unsigned int GetRank(const KeyT& stKey) const
    {
        unsigned int uiRank = 0;
        typename hash_map_type::const_iterator stIT = m_stKeyHash.find(stKey);
        if (stIT != m_stKeyHash.end())
        {
            uiRank = m_stRankList.GetRank(stIT->second);
        }

        return (uiRank);
    }

    unsigned int GetRank(iterator stIT) const
    {
        return m_stRankList.GetRank(stIT);
    }

    inline iterator Find(const KeyT& stKey)
    {
        typename hash_map_type::const_iterator stIT = m_stKeyHash.find(stKey);
        if (stIT != m_stKeyHash.end())
        {
            return stIT->second;
        }

        return End();
    }

    inline iterator FindByRank(unsigned int uiRank) { return (m_stRankList.FindByRank(uiRank)); }

    inline size_type Size() const { return (m_stKeyHash.size()); };

    inline bool Empty() const { return (m_stKeyHash.empty()); };

    inline iterator Begin() { return (m_stRankList.Begin()); };

    inline const_iterator Begin() const { return (m_stRankList.Begin()); };

    inline iterator End() { return (m_stRankList.End()); };

    inline const_iterator End() const { return (m_stRankList.End()); };

    iterator Erase(iterator stIT)
    {
        m_stKeyHash.erase(stIT->first.first);
        return (m_stRankList.Erase(stIT));
    }

    size_type Erase(const KeyT& stKey)
    {
        typename hash_map_type::iterator stIT = m_stKeyHash.find(stKey);
        if (stIT != m_stKeyHash.end())
        {
            m_stRankList.Erase(stIT->second);
            m_stKeyHash.erase(stKey);
        }

        return (m_stKeyHash.size());
    }

private:
    unsigned int                    m_uiMaxRank;
    skip_list_type                  m_stRankList;
    hash_map_type                   m_stKeyHash;
};



