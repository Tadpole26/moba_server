#pragma once

#include "dtype.h"
#include "util_string.h"

#ifdef WIN32
#define FOREGROUND_BLUE				0x0001			//text color contains blue
#define FOREGROUND_GREEN			0x0002			//green
#define FOREGROUND_RED				0x0004			//red
#define FOREGROUND_INTENSITY		0x0008			//intensified
#define BACKGROUND_BLUE				0x0010			//background color contains blue
#define BACKGROUND_GREEN			0x0020			//green
#define	BACKGROUND_RED				0x0040			//red
#define BACKGROUND_INTENSITY		0x0080			//intensified
#define CLR_YELLOW	FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN
#define CLR_RED		FOREGROUND_INTENSITY | FOREGROUND_RED
#define CLR_GREEN	FOREGROUND_INTENSITY | FOREGROUND_GREEN
#define CLR_RESET	FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#else 
#define CLR_RESET		"\033[0m"
#define CLR_BLACK		"\033[30m"		//black
#define CLR_RED			"\033[31m"		//red
#define CLR_REDBACK		"\033[41m"		//red back
#define CLR_GREEN		"\033[32m"		//green
#define CLR_YELLOW		"\033[33m"		//yellow
#define CLR_BLUE		"\033[34m"		//blue
#define CLR_MAGENTA		"\033[35m"		//magenta
#define CLR_CYAN		"\033[36m"		//cyan
#define CLR_WHITE		"\033[37m"		//white
#define CLR_BOLDBLACK		"\033[1m\033[30m"		//bold black
#define CLR_BOLDRED			"\033[1m\033[31m"		//bold red
#define CLR_BOLDGREEN		"\033[1m\033[32m"		//bold green
#define CLR_BOLDYELLOW		"\033[1m\033[33m"		//bold yellow
#define CLR_BOLDBLUE		"\033[1m\033[34m"		//bold blue
#define CLR_BOLDMAGENTA		"\033[1m\033[35m"		//bold magenta
#define CLR_BOLDCYAN		"\033[1m\033[36m"		//bold cyan
#define CLR_BOLDWHITE		"\033[1m\033[37m"		//bold white
#endif 

extern void SetConsoleClr(WORD clr);

template <typename T, typename... Args>
inline void PrintfClr(T clr, const char* szFormat, Args&&... args)
{
#ifdef _WIN32
	SetConsoleClr(clr);
	printf_s(szFormat, std::forward<Args>(args)...);
	SetConsoleClr(CLR_RESET);
	fflush(stdout);
#else
	printf_s("%s", clr);
	printf_s(szFormat, std::forward<Args>(args)...);
	printf_s("%s", CLR_RESET);
	fflush(stdout);
#endif 
}













