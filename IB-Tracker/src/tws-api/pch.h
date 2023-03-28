// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma once

// add headers that you want to pre-compile here

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <windowsx.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <commctrl.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <future>

#include <string>
#include <vector>
#include <algorithm>
#include <tuple>     // std::ignore

#include <objidl.h>
#include <gdiplus.h>


using namespace Gdiplus;
using namespace std::string_literals; // enables 'L' macro

#pragma comment (lib,"Gdiplus.lib")

//' ========================================================================================
//' Simple debug message output to console
//' ========================================================================================
template <typename T>
void dp(T t)
{
    std::wcout << t << std::endl;
}


