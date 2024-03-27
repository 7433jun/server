#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")	
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread>
#include <MSWSock.h>

// Accept 함수 실행시킬 함수 포인터
LPFN_ACCEPTEX lpfnAcceptEx = nullptr;
// Accept용 GUID 설정
GUID guidAcceptEx = WSAID_ACCEPTEX;

SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);



// 비동기 I/O 작업을 처리하는 스레드 함수
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

	// AcceptEx 함수포인터 로드
	DWORD dwBytes;
	
	// 함수 포인터에 값을 넣어줌
	// WSAIoctl(소켓, 제어코드, 입력버퍼, 입력퍼버 크기, 출력버퍼 포인터, 출력버퍼 포인터 크기, 실제 출력 바이트 수, NULL, NULL);
	// WSAIoctl(소켓, 제어코드, &GUID, GUID크기, accept함수포인터, LPFN_ACCEPTEX크기, 실제 출력 바이트 수, NULL, NULL);
	if (WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx),
		&lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl failed with error : %d", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// 클라이언트와 연결 수락할 소켓을 미리 생성
	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	// 만든 소켓이 문제가 있다면
	if (acceptSocket == INVALID_SOCKET)
	{
		printf("Accept Socket failedd with eror : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}








	// IOCP 생성
	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	// 비동기 I/O 작업 처리를 위한 스레드 시작
	thread t(AcceptThread, iocpHandle);

	// key 설정
	ULONG_PTR key = 0;
	// listenSocket이기때문에 acceptSocket과 연결시켜주는것만 한다
	// listenSocket을 IOCP에 연결
	CreateIoCompletionPort((HANDLE)listenSocket, iocpHandle, key, 0);

	char lpOutputBuf[1024];
	// overlapped 초기화
	//WSAOVERLAPPED overlapped;
	//memset(&overlapped, 0, sizeof(overlapped));
	WSAOVERLAPPED overlapped = {};

	// 비동기 Accept
	// lpfnAcceptEx(listen 소켓, accept 소켓, 설정값..., overlapped)
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

	// 스레드 종료 대기
	t.join();

	closesocket(listenSocket);
	WSACleanup();

	return 0;
}