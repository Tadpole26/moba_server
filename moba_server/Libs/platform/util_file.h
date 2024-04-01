#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <chrono>
#include <vector>
#include <sys/stat.h>
#include <ctype.h>
#include "util_string.h"
#include "dtype.h"
#ifdef WIN32
#include <direct.h>
#include <io.h>
#else 
#include <unistd.h>
#include <stdarg.h>
#include <dirent.h>
#endif 

#ifdef WIN32
#define		ACCESS		_access
#define		MKDIR(a)	_mkdir((a))
#else 
#define		ACCESS		access
#define		MKDIR(a)	mkdir((a), 0777)
#endif 

extern int create_dir(const char* src);
extern std::string get_exec_name();
extern std::string get_exec_path();

extern bool exsit_file(const std::string& strFileName);
extern std::string get_local_path(const char* szFile);
