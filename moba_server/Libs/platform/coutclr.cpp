#include "pch.h"
#include "coutclr.h"
#ifdef WIN32
#include <iostream>
#include <windows.h>
#else 
#include <stdio.h>
#include "dirent.h"
#endif

void SetConsoleClr(WORD clr)
{
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
#endif
}