#pragma once
class SocketHelper
{
public:
	static LPFN_ACCEPTEX AcceptEx;
public:
	static bool StartUp();
	static bool CleanUp();
public:
	static bool SetIoControl(SOCKET socket, GUID guid, LPVOID* func);
	static SOCKET CreateSocket();
public:
	// 家南 可记甸
	static bool SetReuseAddress(SOCKET socket, bool enable);
	static bool SetLinger(SOCKET socket, u_short onOff, u_short time);
public:
	static bool Bind(SOCKET socket, SOCKADDR_IN sockAddr);
	static bool Listen(SOCKET socket, int backlog = SOMAXCONN);
	static void CloseSocket(SOCKET& socket);
};

template<typename T>
static inline bool SetSocketOpt(SOCKET socket, int level, int optName, T optVal)
{
	return setsockopt(socket, level, optName, (char*)&optVal, sizeof(T)) != SOCKET_ERROR;
}