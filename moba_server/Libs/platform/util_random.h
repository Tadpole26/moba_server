#pragma once
#include "dtype.h"
#include <math.h>
#include <assert.h>
#include <string>
#include <ctime>
#include "util_time.h"

//最小值~最大值(由于windows下rand的数值范围为0-7fff(32767),所以添加此函数)
extern int rand_x(int max);
extern int rand_range(int range_min, int range_max);
extern double rand_float(double range_min, double range_max);

//生成UUID
extern int64_t GenId();

//32λmd5
extern std::string make_md5_32(const std::string& str);
extern std::string make_md5_32(const char* str, size_t len);
//16λmd5
extern std::string make_md5_16(const std::string& str);
extern std::string make_md5_16(const char* str, size_t len);

