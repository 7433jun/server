#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")	
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread>
#include <MSWSock.h>

// Accept �Լ� �����ų �Լ� ������
LPFN_ACCEPTEX lpfnAcceptEx = nullptr;
// Accept�� GUID ����
GUID guidAcceptEx = WSAID_ACCEPTEX;

SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);



// �񵿱� I/O �۾��� ó���ϴ� ������ �Լ�
void AcceptThread(HANDLE iocpHandle)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	WSAOVERLAPPED* overlapped = {};

	while (true)
	{
		printf("Waiting...\n");
		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
		{
			printf("Client Connected\n");

			char lpOutputBuf[1024];
			SOCKET acceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			DWORD dwBytes;

			if (lpfnAcceptEx(listenSocket, acceptSocket, lpOutputBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR) + 16, &dwBytes, overlapped))
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					printf("AcceptEx failed with error : %d\n", WSAGetLastError());
					closesocket(acceptSocket);
					return 1;
				}
			}
		}
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

	listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
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
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("listening...\n");

	// AcceptEx �Լ������� �ε�
	DWORD dwBytes;
	
	// �Լ� �����Ϳ� ���� �־���
	// WSAIoctl(����, �����ڵ�, �Է¹���, �Է��۹� ũ��, ��¹��� ������, ��¹��� ������ ũ��, ���� ��� ����Ʈ ��, NULL, NULL);
	// WSAIoctl(����, �����ڵ�, &GUID, GUIDũ��, accept�Լ�������, LPFN_ACCEPTEXũ��, ���� ��� ����Ʈ ��, NULL, NULL);
	if (WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx),
		&lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl failed with error : %d", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// Ŭ���̾�Ʈ�� ���� ������ ������ �̸� ����
	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	// ���� ������ ������ �ִٸ�
	if (acceptSocket == INVALID_SOCKET)
	{
		printf("Accept Socket failedd with eror : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}








	// IOCP ����
	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	// �񵿱� I/O �۾� ó���� ���� ������ ����
	thread t(AcceptThread, iocpHandle);

	// key ����
	ULONG_PTR key = 0;
	// listenSocket�̱⶧���� acceptSocket�� ��������ִ°͸� �Ѵ�
	// listenSocket�� IOCP�� ����
	CreateIoCompletionPort((HANDLE)listenSocket, iocpHandle, key, 0);

	char lpOutputBuf[1024];
	// overlapped �ʱ�ȭ
	//WSAOVERLAPPED overlapped;
	//memset(&overlapped, 0, sizeof(overlapped));
	WSAOVERLAPPED overlapped = {};

	// �񵿱� Accept
	// lpfnAcceptEx(listen ����, accept ����, ������..., overlapped)
	if (lpfnAcceptEx(listenSocket, acceptSocket, lpOutputBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR) + 16, &dwBytes, &overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("AcceptEx failed with error : %d\n", WSAGetLastError());
			closesocket(acceptSocket);
			return 1;
		}
	}

	// TODo

	// ������ ���� ���
	t.join();

	closesocket(listenSocket);
	WSACleanup();

	return 0;
}