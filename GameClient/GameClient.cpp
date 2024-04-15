#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")	
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread>
#include <MSWSock.h> 


//IOCP 타입을 만듬
enum  IOCP_TYPE
{
	NONE, 
	CONNECT,
	DISCONNECT,

};

//IocpEvnet을 만들어서 IOCP_TYPE을 추가
struct IocpEvent
{
	WSAOVERLAPPED overlapped = {};	   
	IOCP_TYPE type = NONE;

}; //[WSAOVERLAPPED...   ][IOCP_TYPE.. ]


void ConnectThread(HANDLE iocpHandle)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	//WSAOVERLAPPED overlapped = {};
	IocpEvent* iocpEvent = nullptr;

	while (true)
	{
		printf("Waiting...\n");

		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&iocpEvent, INFINITE))
		{
			switch (iocpEvent->type)
			{
			case CONNECT:
				printf("Client Connect!\n");
				break;
			case DISCONNECT:
				printf("Client Disconnect!\n");
				break;
			default:
				break;
			}
		}
	}

}



int main()
{
	//1초 늦게 시작
	this_thread::sleep_for(1s);

	printf("==== CLIENT ====\n");

	WORD wVersionRequested;
	WSAData wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return 1;
	}

	
	SOCKET connectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	DWORD dwBytes;
	LPFN_CONNECTEX lpfnConnectEx = NULL;
	GUID guidConnectEx = WSAID_CONNECTEX;
	if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), 
		&lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl ConnectEx failed with error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	u_long iMode = 1;


	SOCKADDR_IN serverService;
	memset(&serverService, 0, sizeof(serverService));
	serverService.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serverService.sin_addr);
	serverService.sin_port = htons(27015);

	SOCKADDR_IN clientService;
	memset(&clientService, 0, sizeof(clientService));
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = htonl(INADDR_ANY);
	clientService.sin_port = htons(0); 

	if (bind(connectSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		printf("bind failed with error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}


	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	ULONG_PTR key = 0;
	CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);

	thread t(ConnectThread, iocpHandle);


	DWORD  bytesTransferred = 0;
	IocpEvent* connectEvent = new IocpEvent;
	connectEvent->type = CONNECT;

	if (!lpfnConnectEx(connectSocket, (SOCKADDR*)&serverService, sizeof(serverService), nullptr, 0, &bytesTransferred, &connectEvent->overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("ConnectdEx failed with error : %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

	}

	while (true)
	{

	}


	t.join();
	
	closesocket(connectSocket);
	WSACleanup();

}
