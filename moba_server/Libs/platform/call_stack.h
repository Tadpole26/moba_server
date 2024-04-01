#pragma once

//core后打印信息
#include "dtype.h"
#include <string>
#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
void GetCallStack(CONTEXT& rContext, char* pBuf, size_t size, uint16_t wDepth = 0);
#else		//Linux
#include <execinfo.h>
#include <cxxabi.h>
void* GetCurrentAddress();
void GetCallStack(void* pAddr, char* pBuf, size_t size);
#endif

class TCallStack
{
private:
	TCallStack() {}
	~TCallStack() {}
public:
	static void Stack(char* buf, size_t size);
	static void Stack(std::string& strStack);
};
