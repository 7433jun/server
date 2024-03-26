#include <iostream>
#include <thread>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

struct Session
{
	WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[512] = {};
};

void RecvThread(HANDLE iocpHandle)
{
	DWORD byteTransferred = 0;
	ULONG_PTR key = 0;
	Session* session = nullptr;

	while (true)
	{
		printf("Waiting...\n");
		GetQueuedCompletionStatus(iocpHandle, &byteTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE);

		printf("recv Length : %d\n", byteTransferred);
		printf("recv key : %p\n", key);
	}
}

int main()
{
	printf("==== SERVER ====\n");

	WORD wVersionRequested;
	WSAData wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return 1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(27015);

	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


	printf("listening...\n");

	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	thread t(RecvThread, iocpHandle);

	char recvBuffer[512] = {};

	while (true)
	{
		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		printf("Client Connected\n");

		ULONG_PTR key = 0;
		CreateIoCompletionPort((HANDLE)acceptSocket, iocpHandle, key, 0);

		Session* session = new Session;
		session->socket = acceptSocket;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = sizeof(session->recvBuffer);

		DWORD recvLen = 0;
		DWORD flags = 0;
		WSAOVERLAPPED overlapped = {};

		WSARecv(acceptSocket, OUT & wsaBuf, 1, OUT & recvLen, OUT & flags, &session->overlapped, NULL);
	}

	t.join();

	closesocket(listenSocket);

	WSACleanup();

	return 0;
}