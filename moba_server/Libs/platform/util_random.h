#pragma once
#include "dtype.h"
#include <math.h>
#include <assert.h>
#include <string>
#include <ctime>
#include "util_time.h"

//��Сֵ~���ֵ(����windows��rand����ֵ��ΧΪ0-7fff(32767),������Ӵ˺���)
extern int rand_x(int max);
extern int rand_range(int range_min, int range_max);
extern double rand_float(double range_min, double range_max);

//����UUID
extern int64_t GenId();

//32��md5
extern std::string make_md5_32(const std::string& str);
extern std::string make_md5_32(const char* str, size_t len);
//16��md5
extern std::string make_md5_16(const std::string& str);
extern std::string make_md5_16(const char* str, size_t len);

