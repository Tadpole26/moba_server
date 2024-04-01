#pragma once

#include <stdio.h> //for debug
#include <stddef.h>
#include <functional>
#include "util_random.h"


template <typename T> struct skip_list_node_s
{
    typedef skip_list_node_s<T>             self_type;

    typedef struct skip_list_level_s
    {
        skip_list_level_s()
        {
            m_pForward = NULL;
            m_pBackward = NULL;
            m_uiSpan = 0;
        }

        //该层的下一个节点
        struct skip_list_node_s* m_pForward;
        //该层的前一个节点.主要简化删除时间复杂度
        struct skip_list_node_s* m_pBackward;
        //该层当前节点到该层下一个节点间的节点个数
        unsigned int                m_uiSpan;
    }skip_list_level_t;

    skip_list_node_s() : m_stKey()
    {
        m_uiLevelNum = 0;
        m_pLevel = NULL;
    }

    ~skip_list_node_s()
    {
        delete[] m_pLevel;
    }

    static struct skip_list_node_s* CreateNode(size_t uiLevel, const T& stKey)
    {
        if (0 >= uiLevel)
        {
            uiLevel = 1;
        }

        skip_list_node_s* pNode = new skip_list_node_s();
        pNode->m_stKey = stKey;
        pNode->m_uiLevelNum = uiLevel;
        pNode->m_pLevel = new skip_list_level_t[uiLevel];

        return (pNode);
    }

    //value
    T                               m_stKey;

    //层高
    size_t                          m_uiLevelNum;

    //层数据
    skip_list_level_t* m_pLevel;
};

template <typename T> struct skip_list_iterator
{
    typedef skip_list_iterator<T>                self_type;
    typedef skip_list_node_s<T>                     node_type;

    typedef size_t                                  difference_type;
    typedef std::bidirectional_iterator_tag         iterator_category;
    typedef T                                       value_type;
    typedef T* pointer;
    typedef T& reference;

    skip_list_iterator() : m_pNode() {  };

    skip_list_iterator(node_type* pNode) : m_pNode(pNode) {  };

    reference operator*() const { return (m_pNode->m_stKey); }

    pointer operator->() const { return (&(m_pNode->m_stKey)); }

    self_type& operator++()
    {
        m_pNode = m_pNode->m_pLevel[0].m_pForward;
        return (*this);
    }

    self_type operator++(int)
    {
        self_type stTmp = *this;
        m_pNode = m_pNode->m_pLevel[0].m_pForward;
        return (stTmp);
    }

    self_type& operator--()
    {
        m_pNode = m_pNode->m_pLevel[0].m_pBackward;
        return (*this);
    }

    self_type operator--(int)
    {
        self_type stTmp = *this;
        m_pNode = m_pNode->m_pLevel[0].m_pBackward;
        return (stTmp);
    }

    bool operator==(const self_type& stIter) const { return (m_pNode == stIter.m_pNode); }

    bool operator!=(const self_type& stIter) const { return (m_pNode != stIter.m_pNode); }

    node_type* m_pNode;
};

template <typename T> struct skip_list_const_iterator
{
    typedef skip_list_const_iterator<T>             self_type;
    typedef skip_list_node_s<T>                     node_type;
    typedef skip_list_iterator<T>                   iterator;

    typedef size_t                                  difference_type;
    typedef std::bidirectional_iterator_tag         iterator_category;
    typedef T                                       value_type;
    typedef const T* pointer;
    typedef const T& reference;

    skip_list_const_iterator() : m_pNode() {  };

    skip_list_const_iterator(const node_type* pNode) : m_pNode(pNode) {  };

    skip_list_const_iterator(const iterator& stIter) : m_pNode(stIter.m_pNode) {  };

    reference operator*() const { return (m_pNode->m_stKey); }

    pointer operator->() const { return (&(m_pNode->m_stKey)); }

    self_type& operator++()
    {
        m_pNode = m_pNode->m_pLevel[0].m_pForward;
        return (*this);
    }

    self_type operator++(int)
    {
        self_type stTmp = *this;
        m_pNode = m_pNode->m_pLevel[0].m_pForward;
        return (stTmp);
    }

    self_type& operator--()
    {
        m_pNode = m_pNode->m_pLevel[0].m_pBackward;
        return (*this);
    }

    self_type operator--(int)
    {
        self_type stTmp = *this;
        m_pNode = m_pNode->m_pLevel[0].m_pBackward;
        return (stTmp);
    }

    bool operator==(const self_type& stIter) const { return (m_pNode == stIter.m_pNode); }

    bool operator!=(const self_type& stIter) const { return (m_pNode != stIter.m_pNode); }

    const node_type* m_pNode;
};

template <typename T, class Comparator = std::less<T> >
class CTCSkipList
{
public:
    static constexpr size_t MAX_SKIPLIST_LEVEL = 32;
    static constexpr float LEVEL_PROBABILITY = 0.3; //多一层level的概率

public:
    typedef T                                       key_type;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    typedef key_type* pointer;
    typedef const key_type* const_pointer;
    typedef const key_type& const_reference;
    typedef skip_list_node_s<T>                     node_type;
    typedef skip_list_iterator<T>                   iterator;
    typedef skip_list_const_iterator<T>             const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

public:
    CTCSkipList()
    {
        m_pHeader = skip_list_node_s<T>::CreateNode(MAX_SKIPLIST_LEVEL, key_type());
        for (size_t i = 0; i < MAX_SKIPLIST_LEVEL; i++)
        {
            m_pHeader->m_pLevel[i].m_pForward = m_pHeader;
            m_pHeader->m_pLevel[i].m_pBackward = m_pHeader;
            m_pHeader->m_pLevel[i].m_uiSpan = 0;
        }

        m_uiLength = 0;
        m_uiLevel = 1;
    }

    ~CTCSkipList()
    {
        node_type* pNode = m_pHeader->m_pLevel[0].m_pForward, * pNext = NULL;

        while (pNode != m_pHeader)
        {
            pNext = pNode->m_pLevel[0].m_pForward;
            delete pNode;
            pNode = pNext;
        }

        delete m_pHeader;
    }

    iterator Begin() { return (iterator(m_pHeader->m_pLevel[0].m_pForward)); };

    const_iterator Begin() const { return (const_iterator(m_pHeader->m_pLevel[0].m_pForward)); };

    iterator End() { return (iterator(m_pHeader)); };

    const_iterator End() const { return (const_iterator(m_pHeader)); };

    reverse_iterator RBegin() { return (reverse_iterator(End())); };

    const_reverse_iterator RBegin() const { return (const_reverse_iterator(End())); };

    reverse_iterator REnd() { return (reverse_iterator(Begin())); };

    const_reverse_iterator REnd() const { return (const_reverse_iterator(Begin())); };

    size_type Size() const { return (m_uiLength); };

    bool Empty() const { return (m_uiLength == 0); };

    //rank and iterator
    std::pair<unsigned int, iterator> Insert(const T& stKey)
    {
        node_type* pUpdateNodeList[MAX_SKIPLIST_LEVEL], * pTmp;
        unsigned int stRankStep[MAX_SKIPLIST_LEVEL];

        pTmp = m_pHeader;
        for (int i = (int)(m_uiLevel - 1); i >= 0; i--)
        {
            stRankStep[i] = i == (int)(m_uiLevel - 1) ? 0 : stRankStep[i + 1];
            while ((m_pHeader != pTmp->m_pLevel[i].m_pForward) && (m_stCmp(stKey, pTmp->m_pLevel[i].m_pForward->m_stKey) == false))
            {
                stRankStep[i] += pTmp->m_pLevel[i].m_uiSpan;
                pTmp = pTmp->m_pLevel[i].m_pForward;
            }

            pUpdateNodeList[i] = pTmp;
        }

        unsigned int uiLevel = GetRandomLevel();
        if (uiLevel > m_uiLevel)
        {
            for (unsigned int i = m_uiLevel; i < uiLevel; i++)
            {
                stRankStep[i] = 0;
                pUpdateNodeList[i] = m_pHeader;
                pUpdateNodeList[i]->m_pLevel[i].m_uiSpan = m_uiLength;
            }

            m_uiLevel = uiLevel;
        }

        pTmp = skip_list_node_s<T>::CreateNode(uiLevel, stKey);
        for (unsigned int i = 0; i < uiLevel; i++)
        {
            pTmp->m_pLevel[i].m_pForward = pUpdateNodeList[i]->m_pLevel[i].m_pForward;
            pTmp->m_pLevel[i].m_pBackward = pUpdateNodeList[i];
            pUpdateNodeList[i]->m_pLevel[i].m_pForward = pTmp;
            pTmp->m_pLevel[i].m_pForward->m_pLevel[i].m_pBackward = pTmp;

            //update span covered by update[i] as pTmp is inserted here
            pTmp->m_pLevel[i].m_uiSpan = pUpdateNodeList[i]->m_pLevel[i].m_uiSpan - (stRankStep[0] - stRankStep[i]);
            pUpdateNodeList[i]->m_pLevel[i].m_uiSpan = stRankStep[0] - stRankStep[i] + 1;
        }

        for (unsigned int i = uiLevel; i < m_uiLevel; i++)
        {
            pUpdateNodeList[i]->m_pLevel[i].m_uiSpan++;
        }

        m_uiLength++;

        return (std::pair<unsigned int, iterator>(stRankStep[0] + 1, iterator(pTmp)));
    }

    std::pair<unsigned int, iterator> FindFirst(const T& stKey)
    {
        node_type* pTmp = m_pHeader, * pNearest = m_pHeader;
        unsigned int uiRank = 0, uiNearestRank = 0;

        pTmp = m_pHeader;
        for (int i = (int)(m_uiLevel - 1); i >= 0; i--)
        {
            while ((m_pHeader != pTmp->m_pLevel[i].m_pForward) && (m_stCmp(pTmp->m_pLevel[i].m_pForward->m_stKey, stKey) == true))
            {
                uiRank += pTmp->m_pLevel[i].m_uiSpan;
                pTmp = pTmp->m_pLevel[i].m_pForward;
            }

            //if equal,record the nearest equal node
            if ((m_pHeader != pTmp->m_pLevel[i].m_pForward) && (m_stCmp(stKey, pTmp->m_pLevel[i].m_pForward->m_stKey) == false))
            {
                uiNearestRank = uiRank + pTmp->m_pLevel[i].m_uiSpan;
                pNearest = pTmp->m_pLevel[i].m_pForward;
            }
        }

        return (std::pair<unsigned int, iterator>(uiNearestRank, iterator(pNearest)));
    }

    std::pair<unsigned int, iterator> FindLast(const T& stKey)
    {
        node_type* pTmp = m_pHeader, * pFarthest = m_pHeader;
        unsigned int uiRank = 0, uiFarthest = 0;

        pTmp = m_pHeader;
        for (int i = (int)(m_uiLevel - 1); i >= 0; i--)
        {
            while ((m_pHeader != pTmp->m_pLevel[i].m_pForward) && (m_stCmp(stKey, pTmp->m_pLevel[i].m_pForward->m_stKey) == false))
            {
                uiRank += pTmp->m_pLevel[i].m_uiSpan;
                pTmp = pTmp->m_pLevel[i].m_pForward;

                //if equal
                if (m_stCmp(pTmp->m_stKey, stKey) == false)
                {
                    uiFarthest = uiRank;
                    pFarthest = pTmp;
                }
            }
        }

        return (std::pair<unsigned int, iterator>(uiFarthest, iterator(pFarthest)));
    }

    iterator Erase(iterator stIT)
    {
        if (stIT == End())
        {
            return (End());
        }

        iterator stNextIT(stIT.m_pNode->m_pLevel[0].m_pForward);
        for (size_t i = 0; i < stIT.m_pNode->m_uiLevelNum; i++)
        {
            stIT.m_pNode->m_pLevel[i].m_pBackward->m_pLevel[i].m_pForward = stIT.m_pNode->m_pLevel[i].m_pForward;
            stIT.m_pNode->m_pLevel[i].m_pForward->m_pLevel[i].m_pBackward = stIT.m_pNode->m_pLevel[i].m_pBackward;

            //减去要删除节点
            unsigned int uiBackwardSpan = stIT.m_pNode->m_pLevel[i].m_pBackward->m_pLevel[i].m_uiSpan - 1;
            unsigned int uiSelfSpan = stIT.m_pNode->m_pLevel[i].m_uiSpan;
            stIT.m_pNode->m_pLevel[i].m_pBackward->m_pLevel[i].m_uiSpan = uiBackwardSpan + uiSelfSpan;
        }

        if ((stIT.m_pNode->m_uiLevelNum == m_uiLevel) &&
            (stIT.m_pNode->m_pLevel[stIT.m_pNode->m_uiLevelNum - 1].m_pBackward == m_pHeader) &&
            (stIT.m_pNode->m_pLevel[stIT.m_pNode->m_uiLevelNum - 1].m_pForward == m_pHeader))
        {
            m_uiLevel--;
        }
        else
        {
            node_type* pTmp = stIT.m_pNode->m_pLevel[stIT.m_pNode->m_uiLevelNum - 1].m_pBackward;
            unsigned int uiLevelNum = stIT.m_pNode->m_uiLevelNum;
            while (uiLevelNum < m_uiLevel)
            {
                if (pTmp->m_uiLevelNum > uiLevelNum)
                {
                    pTmp->m_pLevel[uiLevelNum].m_uiSpan--;
                    uiLevelNum++;
                }
                else
                {
                    pTmp = pTmp->m_pLevel[pTmp->m_uiLevelNum - 1].m_pBackward;
                }
            }
        }

        delete stIT.m_pNode;
        m_uiLength--;

        return (stNextIT);
    }

    unsigned int GetRank(const_iterator stIT) const
    {
        unsigned int uiRank = 0;
        if (stIT != End())
        {
            const node_type* pTmp = stIT.m_pNode;
            while (m_pHeader != pTmp)
            {
                uiRank += pTmp->m_pLevel[pTmp->m_uiLevelNum - 1].m_pBackward->m_pLevel[pTmp->m_uiLevelNum - 1].m_uiSpan;
                pTmp = pTmp->m_pLevel[pTmp->m_uiLevelNum - 1].m_pBackward;
            }
        }

        return (uiRank);
    }

    unsigned int GetRank(iterator stIT)
    {
        unsigned int uiRank = 0;
        if (stIT != End())
        {
            node_type* pTmp = stIT.m_pNode;
            while (m_pHeader != pTmp)
            {
                uiRank += pTmp->m_pLevel[pTmp->m_uiLevelNum - 1].m_pBackward->m_pLevel[pTmp->m_uiLevelNum - 1].m_uiSpan;
                pTmp = pTmp->m_pLevel[pTmp->m_uiLevelNum - 1].m_pBackward;
            }
        }

        return (uiRank);
    }

    iterator FindByRank(unsigned int uiRank)
    {
        if ((0 == m_uiLength) || (0 == uiRank) || (uiRank > m_uiLength))
        {
            return (End());
        }

        unsigned int uiLeftRank = uiRank;

        node_type* pTmp = m_pHeader;
        for (int i = (int)(m_uiLevel - 1); i >= 0; i--)
        {
            while (uiLeftRank >= pTmp->m_pLevel[i].m_uiSpan)
            {
                uiLeftRank -= pTmp->m_pLevel[i].m_uiSpan;
                pTmp = pTmp->m_pLevel[i].m_pForward;

                if (0 == uiLeftRank) break;
            }

            if (0 == uiLeftRank) break;
        }

        return (iterator(pTmp));
    }

    void Dump()
    {
        std::string strSep = "|";
        printf("LENGTH:%u|LEVEL:%u\n", m_uiLength, m_uiLevel);
        printf("rnk%s", strSep.c_str());
        node_type* pTmp = m_pHeader->m_pLevel[0].m_pForward;
        unsigned int uiRank = 1;
        while (pTmp != m_pHeader)
        {
            printf("%3u%s", uiRank, strSep.c_str());
            uiRank++;
            pTmp = pTmp->m_pLevel[0].m_pForward;
        }
        printf("\n");

        printf("num%s", strSep.c_str());
        pTmp = m_pHeader->m_pLevel[0].m_pForward;
        while (pTmp != m_pHeader)
        {
            printf("%3u%s", (unsigned int)pTmp->m_stKey, strSep.c_str());
            pTmp = pTmp->m_pLevel[0].m_pForward;
        }

        printf("\n");

        for (unsigned int i = 0; i < m_uiLevel; i++)
        {
            printf("%3u%s", m_pHeader->m_pLevel[i].m_uiSpan, strSep.c_str());
            pTmp = m_pHeader->m_pLevel[i].m_pForward;
            node_type* pCurrentNode = m_pHeader->m_pLevel[0].m_pForward;
            while (pTmp != m_pHeader)
            {
                while (pCurrentNode != pTmp)
                {
                    printf("   %s", strSep.c_str());
                    pCurrentNode = pCurrentNode->m_pLevel[0].m_pForward;
                }

                printf("%3u%s", pTmp->m_pLevel[i].m_uiSpan, strSep.c_str());
                pTmp = pTmp->m_pLevel[i].m_pForward;
                pCurrentNode = pCurrentNode->m_pLevel[0].m_pForward;
            }

            printf("\n");
        }
    }

private:
    unsigned int GetRandomLevel()
    {
        size_t uiLevel = 1;
        while ((taf::random() & 0xFFFF) < (unsigned int)(LEVEL_PROBABILITY * 0xFFFF))
        {
            if ((++uiLevel) >= (unsigned int)MAX_SKIPLIST_LEVEL)
            {
                break;
            }
        }

        return (uiLevel);
    }

private:
    Comparator                      m_stCmp;
    node_type* m_pHeader;
    size_type                       m_uiLength;
    unsigned int                    m_uiLevel;
};



