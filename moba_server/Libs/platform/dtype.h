#pragma once

//windows
#if defined(WIN32) && !defined(DEV_WIN32)
using UINT64 = unsigned __int64;
using INT64 = signed __int64;
using uint64 = unsigned __int64;
using int64 = signed __int64;
using DWORD = unsigned long;
#else //linux
using UINT64 = unsigned long long int;
using INT64 = signed long long int;
using uint64 = unsigned long long int;
using int64 = signed long long int;
using __int64 = signed long long int;
using DWORD = unsigned long;
#endif 

using UL_PTR = uint64;
using BOOL = int;
using BYTE = unsigned char;
using WORD = unsigned short;
using INT8 = signed char;
using int8 = signed char;
using uchar = unsigned char;
using UINT8 = unsigned char;
using uint8 = unsigned char;
using INT16 = signed short int;
using int16 = signed short int;
using UINT16 = unsigned short int;
using uint16 = unsigned short int;
using INT32 = int;
using int32 = int;
using UINT32 = unsigned int;
using uint32 = unsigned int;
using UINT = unsigned int;
using CHAR = char;
using FLOAT = float;

#include <map>
#include <unordered_map>
#include <set>
#include <math.h>
#include <algorithm>

#define MAKE_INT64(hi, lo) ((((int64_t)(hi)) << 32) | (int64_t)(lo))
#define MAKE_UINT64(hi, lo) ((((uint64_t)(hi)) << 32) | (uint64_t)(lo))
#define MAKE_INT32(hi, lo) ((((int32_t)(hi)) << 16) | (int32_t)(lo))
#define MAKE_UINT32(hi, lo) ((((uint32_t)(hi)) << 32) | (uint32_t)(lo))
#define MAKE_UINT16(hi, lo) ((((uint16_t)(hi)) << 8) | (uint16_t)(lo))

#define FILT_HIGH32(id) ((id) >> 32)
#define FILT_LOW32(id) (0x00000000FFFFFFFF & (id))
#define FILT_HIGH16(id) ((id) >> 16)
#define FILT_LOW16(id) (0x0000FFFF&(id))
#define FILT_HIGH8(id) ((id) >> 8)
#define FILT_LOW8(id) (0x00FF&(id))

#define MK_MOD_INT(hi, lo, n)((hi)*(int64)pow(10, (n)) + (lo))
#define FLT_MOD_HIGH(id, n) ((id)/(int64)pow(10, (n)))
#define FLT_MOD_LOW(id, n) ((id)%(int64)pow(10, (n)))

#define DEF_C(N, S, C) const char* N = S;

#define MAX_SIZE_256M 268435456		//256*1024*1024
#define MAX_SIZE_512M 536870912		//512*1024*1024
#define MAX_SIZE_1M 1048575			//1024*1024
#define MAX_SIZE_256K 262144		//1024*256
#define MAX_SIZE_128K 131072		//128*1024
#define MAX_SIZE_64K 65536			//64*1024
#define MAX_SIZE_32K 32768			//32*1024







