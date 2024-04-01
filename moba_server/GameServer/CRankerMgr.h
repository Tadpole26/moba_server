#pragma once

#include "singleton.h"
#include "rank_map.h"

///’Ω∂∑¡¶≈≈––
struct fight_power_rank_value_t {};

typedef RankMap<int64_t, fight_power_rank_value_t> FIGHT_POWER_RANK_MAP_TYPE;

#define FIGHT_POWER_RANK_INS Singleton<FIGHT_POWER_RANK_MAP_TYPE>::getInstance()
