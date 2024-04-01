#include <iostream>
#include <thread>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

// IOCP를 사용하여 수신된 데이터를 처리하는 쓰레드 함수
void RecvThread(HANDLE iocpHandle)
{
	while (true)
	{
		printf("Hello\n");

		this_thread::sleep_for(100ms);
	}
}

int main()
{
	printf("==== SERVER ====\n");

	WORD wVersionRequested;
	WSAData wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	// 생성
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return 1;
	}

	// 리슨소켓 생성
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// 주소 생성
	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(27015);

	// listenSocket에 주소 등록
	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// 수신 대기
	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


	printf("listening...\n");

	// IOCP 핸들 생성 및 초기화
	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	// 스레드 만들어서 해당 함수 호출
	// 매개변수로 CreateIoCompletionPort로 만들어진 새 핸들
	thread t(RecvThread, iocpHandle);

	// 수신 데이터 버퍼
	char recvBuffer[512] = {};


	while (true)
	{
		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			closesocket(listenSocket); // 오류시 listenSocket 닫음
			WSACleanup();
			return 1;
		}

		printf("Client Connected\n"); // 클라이언트 연결 성공

		// key 값은 0
		ULONG_PTR key = 0;
		// 클라이언트 소켓을 IOCP에 연결
		CreateIoCompletionPort((HANDLE)acceptSocket, iocpHandle, key, 0);

		// 수신 버퍼 설정
		WSABUF wsaBuf;
		wsaBuf.buf = recvBuffer;			// 받을 메모리 공간
		wsaBuf.len = sizeof(recvBuffer);	// 받을 메모리 공간 크기

		DWORD recvLen = 0;					// 수신된 데이터 길이를 저장할 변수
		DWORD flags = 0;					// 플래그 변수, 현재는 사용하지 않음
		WSAOVERLAPPED overlapped = {};		// WSAOVERLAPPED 구조체 할당후 초기화, 비동기 I/O 작업에 사용

		// WSARecv
		// s : 소켓. 위에 acceptSocket 넣어 주면됨
		// lpBuffers : WSABUF 구조체 배열에 대한 포인터
		// dwBufferCount : WSABUF 구조체 갯수
		// lpNumberOfButesRecvd : 수신 완료 후 받은 바이트에 대한 포인터
		// lpFlags : 플래그
		// lpOverlapped : WSAOVERLAPPED 구조체 포인터
		// lpCompletionRoutine : 수신 완료 후 호출되는 콜백함수
		WSARecv(acceptSocket, OUT & wsaBuf, 1, OUT & recvLen, OUT & flags, &overlapped, NULL);
	}

	// 스레드 완료까지 대기
	t.join();

	// 소켓 닫기
	closesocket(listenSocket);
	// 해제
	WSACleanup();

	return 0;
}