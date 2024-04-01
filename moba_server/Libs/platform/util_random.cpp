#include "pch.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else 
#include <stdarg.h>
#include <sys/stat.h>
#endif 

#include "util_random.h"
#include "md5.h"

int rand_x(int range_max)
{
	if (range_max == 0) return 0;

	return ((((unsigned int)rand() << 24) & 0xFF000000)
		| (((unsigned int)rand() << 12) & 0x00FFF000)
		| (((unsigned int)rand()) & 0x00000FFF)) % range_max;
}

int rand_range(int range_min, int range_max)
{
	if (range_min == range_max)
		return range_min;
	else 
		return ((((unsigned int)rand() << 24) & 0xFF000000)
			| (((unsigned int)rand() << 12) & 0x00FFF000)
			| (((unsigned int)rand()) & 0x00000FFF)) % (range_max - range_min) + range_min;
}

double rand_float(double range_min, double range_max)
{
	return ((double(rand()) / double(RAND_MAX)) * (range_max - range_min)) + range_min;
}

int64_t GenId()
{
	static uint32_t g_id = 0;
	int64_t ret = GetMillSec();
	ret = ret * 100000 + g_id;
	++g_id;
	return ret;
}

std::string make_md5_32(const std::string& str)
{
	MD5 md5;
	md5.update(str);
	return md5.toString();
}

std::string make_md5_32(const char* str, size_t len)
{
	MD5 md5;
	md5.update(str, len);
	return md5.toString();
}
//16¦Ëmd5
std::string make_md5_16(const std::string& str)
{
	std::string ret = make_md5_32(str);
	ret = ret.substr(8, 16);
	return ret;
}
std::string make_md5_16(const char* str, size_t len)
{
	std::string ret = make_md5_32(str, len);
	ret = ret.substr(8, 16);
	return ret;
}

