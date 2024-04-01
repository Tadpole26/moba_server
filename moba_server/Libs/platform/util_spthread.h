#pragma once


#ifndef WIN32
	#define SP_THREAD_CALL
#else 
	#define SP_THREAD_CALL __stdcall 
#endif 