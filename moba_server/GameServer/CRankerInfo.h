#pragma once
#include "record_associate_container.h"
#include "CGlobalRankerRecord.h"

class CRankerInfo
{
public:
    CRankerInfo() {}

public:
    using ranker_container_type = RecordAssociateContainer<int32_t, global_ranker_table_t*, global_ranker_table_value_type>;

    ranker_container_type                             m_mapGlobalRanker;                  //ÅÅÐÐ°ñÊý¾Ý
};
