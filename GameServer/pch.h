#pragma once

#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib") // Debug모드일때
#else
#pragma comment(lib, "Release\\ServerCore.lib") // Release모드일때
#endif

#include <CorePch.h>
