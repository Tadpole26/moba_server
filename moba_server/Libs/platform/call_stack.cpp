#include "pch.h"
#include <stdio.h>
#include <memory>
#include <cstring>
#include "call_stack.h"
#include "util_string.h"

void TCallStack::Stack(char* buf, size_t size)
{
	if (size <= 0) return;
#ifdef _WIN32
	CONTEXT c;
	RtlCaptureContext(&c);
	GetCallStack(c, buf, size);
#else 
	GetCallStack(GetCurrentAddress(), buf, size);
#endif
	buf[size - 1] = 0;
}

void TCallStack::Stack(std::string& strStack)
{
	strStack.clear();
	char szStack[2048];
	Stack(szStack, 2048);
	szStack[2047] = 0;
	strStack.append(szStack, strlen(szStack));
}

#ifdef _WIN32
void GetCallStack(CONTEXT& rContext, char* pBuf, size_t size, uint16_t wDepth)
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();
	if (!SymInitialize(hProcess, NULL, TRUE))
	{
		sprintf_s(pBuf, size, "GetCallStack fails!");
		return;
	}

	static STACKFRAME64 sf;
	memset(&sf, 0, sizeof(STACKFRAME64));
#ifdef WIN64
	sf.AddrPC.Offset = rContext.Rip;
	sf.AddrStack.Offset = rContext.Rsp;
	sf.AddrFrame.Offset = rContext.Rbp;
	uint32 machineType = IMAGE_FILE_MACHINE_AMD64;
#else 
	sf.AddrPC.Offset = rContext.Rip;
	sf.AddrStack.Offset = rContext.Rsp;
	sf.AddrFrame.Offset = rContext.Rbp;
	uint32 machineType = IMAGE_FILE_MACHINE_I386;
#endif
	sf.AddrPC.Mode = AddrModeFlat;
	sf.AddrStack.Mode = AddrModeFlat;
	sf.AddrFrame.Mode = AddrModeFlat;

	static char szBuf1[0x100], szBuf2[0x100];
	uint64 qwDisplacement = 0;
	unsigned long dwLineDisplacement = 0;
	int offset = 0, ret = 0;
	for (uint16_t i = 0; wDepth == 0 || i < wDepth; ++i)
	{
		if (i > 100) break;

		if (!StackWalk64(machineType, hProcess, hThread, &sf, &rContext, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0)) break;

		if (sf.AddrFrame.Offset == 0) break;

		static ULONG64 buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1 / sizeof(ULONG64)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;

		if (SymFromAddr(hProcess, sf.AddrPC.Offset, &qwDisplacement, pSymbol))
			ret = sprintf_s(pBuf + offset, size - offset, "[%s] at", pSymbol->Name);
		else
			ret = sprintf_s(pBuf + offset, size - offset, "[error] at");
		if (ret > 0)
			offset += ret;

		static IMAGEHLP_LINE64 lineInfo = { sizeof(IMAGEHLP_LINE64) };
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo))
			ret = sprintf_s(pBuf + offset, size - offset, "[%s Line: %u]\n", lineInfo.FileName, lineInfo.LineNumber);
		else
			ret = sprintf_s(pBuf + offset, size - offset, "[error Line: 0]\n");
		if (ret > 0)
			offset += ret;

		if (offset + 0x100 > (int)size)
			break;
	}
	SymCleanup(hProcess);
}
#else 
void* GetCurrentAddress()
{
	return __builtin_return_address(0);
}

void GetCallStack(void* pAddr, char* pBuf, size_t size_)
{
#define BUFFER_SIZE 100
	void* arBuf[BUFFER_SIZE];
	int size = backtrace(arBuf, BUFFER_SIZE);
	arBuf[1] = pAddr;
	int offset = 0, ret = 0;
	char** messages = backtrace_symbols(arBuf, size);
	if (messages == NULL)
	{
		printf("messages is NULL!\n");
		return;
	}
	for (int i = 1; i < size && messages != NULL; ++i)
	{
		char* pMangledName = NULL, * pOffsetBegin = NULL, * pOffsetEnd = NULL;
		for (char* p = messages[i]; *p; ++p)
		{
			if (*p == '(')
				pMangledName = p;
			else if (*p == '+')
				pOffsetBegin = p;
			else if (*p == ')')
			{
				pOffsetEnd = p;
				break;
			}
		}
		if (pMangledName && pOffsetBegin && pOffsetEnd && pMangledName < pOffsetBegin)
		{
			*pMangledName++ = '\0';
			*pOffsetBegin++ = '\0';
			*pOffsetEnd++ = '\0';

			int status = 0;
			char* pRealName = abi::__cxa_demangle(pMangledName, 0, 0, &status);

			if (status == 0)
				ret = sprintf_s(pBuf + offset, size - offset,
					"[bt]: (%u) %s : %s+%s%s\n", i, messages[i],
					pRealName, pOffsetBegin, pOffsetEnd);
			else
				ret = sprintf_s(pBuf + offset, size - offset,
					"[bt]: (%u) %s : %s+%s%s\n", i, messages[i],
					pMangledName, pOffsetBegin, pOffsetEnd);
			free(pRealName);
		}
		else
			ret = sprintf_s(pBuf + offset, size - offset, "[bt]: (%u) %s\n", i, messages[i]);
		if (ret > 0)
			offset += ret;
		else
			printf("offset error!\n");
		if (offset + 0x100 > (int)size_)
			break;
	}
#ifdef _DEBUG
	size_t len = strlen(pBuf);
	if (len < 5)
		printf("length error!\n");
#endif 
	free(messages);
}
#endif
