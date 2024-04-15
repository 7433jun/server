#pragma once
#include <iostream>
using namespace std;

#pragma region STL

#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#pragma endregion

#pragma region Socket

#pragma comment(lib, "Ws2_32.lib")	
#include <WinSock2.h>	
#include <WS2tcpip.h> 
#include <MSWSock.h> 

#pragma endregion

#pragma region thread

#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>

#pragma endregion

#pragma region Types

//C++ 11
using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

#pragma endregion



