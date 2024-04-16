#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")	
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread>

struct Session
{
	WSAOVERLAPPED overlapped = {};		// 비동기 I/O 작업을 위한 구조체
	SOCKET socket = INVALID_SOCKET;		// 클라이언트와의 통신을 담당하는 소켓
	char sendBuffer[512] = {};			// 데이터 송신을 위한 버퍼
};

void SendThread(HANDLE iocpHandle)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	Session* session = nullptr;

	while (true)
	{
		printf("Waiting...");
		//보낼 준비가 된다면
		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&session, INFINITE))
		{
			WSABUF wsaBuf;
			wsaBuf.buf = session->sendBuffer;			// 송신 버퍼 지정
			wsaBuf.len = sizeof(session->sendBuffer); 	// 버퍼의 크기 지정

			DWORD sendLen = 0;	// 송신 데이터 길이를 저장할 변수
			DWORD flags = 0;	// flag, 현재 사용하지 않음

			printf("session->sendBuffer : %s", session->sendBuffer);

			//비동기로 보냄
			if (WSASend(session->socket, &wsaBuf, 1, &sendLen, flags, &session->overlapped, NULL) == SOCKET_ERROR)
			{
				//연결 실패시 오류 메세지 출력
				printf("send failed with error %d", WSAGetLastError());
				return;
			}

			printf("Send...\n");

		}

		//1초에 한번씩 작동
		this_thread::sleep_for(1s);
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

	SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(27015);

	if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{

		printf("connect function failed with error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;

	}

	//서버와 연결 성공
	printf("Connected\n");

	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	thread t(SendThread, iocpHandle);

	ULONG_PTR key = 0;

	//connectSocket을 iocpHandle이랑 연결
	CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);

	Session* session = new Session;
	session->socket = connectSocket;
	char sendBuffer[512] = "Hello thils is Client"; // 전송할 메세지 설정

	//session->sendBuffer에 보내고 싶은 자료 복사
	//memcpy(담을곳, 복사할 시작주소, 복사할 크기)
	memcpy(session->sendBuffer, sendBuffer, sizeof(sendBuffer));

	WSABUF wsaBuf;
	wsaBuf.buf = session->sendBuffer;
	wsaBuf.len = sizeof(session->sendBuffer);

	DWORD sendLen = 0;
	DWORD flags = 0;

	//WSASend 호출
	WSASend(connectSocket, &wsaBuf, 1, &sendLen, flags, &session->overlapped, NULL);

	t.join();

	closesocket(connectSocket);
	WSACleanup();

}
