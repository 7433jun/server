#pragma once
#include <iostream>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")	
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread>
#include <MSWSock.h> 

//C++ 11
//using BYTE = unsigned char;
//using int8 = __int8;  
//using int16 = __int16;
//using int32 = __int32;
//using int64 = __int64;
//using uint8 = unsigned __int8;
//using uint16 = unsigned __int16;
//using uint32 = unsigned __int32;
//using uint64 = unsigned __int64;

typedef	unsigned char BYTE;
typedef	__int8 int8;
typedef	__int16	int16;
typedef	__int32	int32;
typedef	__int64	int64;
typedef	unsigned __int8	uint8;
typedef	unsigned __int16 uint16;
typedef	unsigned __int32 uint32;
typedef	unsigned __int64 uint64;