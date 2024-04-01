#pragma once

#ifndef _WIN32
	#ifndef _NJEMALLOC_
		#define _JEMALLOC_
	#endif
#endif

#ifdef _JEMALLOC_
	#ifndef JEMALLOC_NO_DEMANGLE
		#define JEMALLOC_NO_DEMANGLE
	#endif 
	#include "jemalloc.h"
#endif

inline void* msg_alloc(size_t nSize)
{
#ifdef _JEMALLOC_
	return je_malloc(nSize);
#else 
	return malloc(nSize);
#endif
}

inline void* msg_relloc(void* pMsg, size_t nSize)
{
#ifdef _JEMALLOC_
	return je_realloc(pMsg, nSize);
#else 
	return realloc(pMsg, nSize);
#endif
}

inline void* msg_calloc(size_t num, size_t size)
{
#ifdef _JEMALLOC_
	return je_calloc(num, size);
#else 
	return calloc(num, size);
#endif 
}

inline void msg_free(void* pMsg)
{
#ifdef _JEMALLOC_
	je_free(pMsg);
#else 
	free(pMsg);
#endif 
}

#define MSGFREE(ptr) if (ptr) { msg_free(ptr); (ptr) = NULL; }







