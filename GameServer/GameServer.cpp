#include <iostream>
#include <thread>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

// IOCP�� ����Ͽ� ���ŵ� �����͸� ó���ϴ� ������ �Լ�
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

	// ����
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return 1;
	}

	// �������� ����
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// �ּ� ����
	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(27015);

	// listenSocket�� �ּ� ���
	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// ���� ���
	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


	printf("listening...\n");

	// IOCP �ڵ� ���� �� �ʱ�ȭ
	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	// ������ ���� �ش� �Լ� ȣ��
	// �Ű������� CreateIoCompletionPort�� ������� �� �ڵ�
	thread t(RecvThread, iocpHandle);

	// ���� ������ ����
	char recvBuffer[512] = {};


	while (true)
	{
		SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			closesocket(listenSocket); // ������ listenSocket ����
			WSACleanup();
			return 1;
		}

		printf("Client Connected\n"); // Ŭ���̾�Ʈ ���� ����

		// key ���� 0
		ULONG_PTR key = 0;
		// Ŭ���̾�Ʈ ������ IOCP�� ����
		CreateIoCompletionPort((HANDLE)acceptSocket, iocpHandle, key, 0);

		// ���� ���� ����
		WSABUF wsaBuf;
		wsaBuf.buf = recvBuffer;			// ���� �޸� ����
		wsaBuf.len = sizeof(recvBuffer);	// ���� �޸� ���� ũ��

		DWORD recvLen = 0;					// ���ŵ� ������ ���̸� ������ ����
		DWORD flags = 0;					// �÷��� ����, ����� ������� ����
		WSAOVERLAPPED overlapped = {};		// WSAOVERLAPPED ����ü �Ҵ��� �ʱ�ȭ, �񵿱� I/O �۾��� ���

		// WSARecv
		// s : ����. ���� acceptSocket �־� �ָ��
		// lpBuffers : WSABUF ����ü �迭�� ���� ������
		// dwBufferCount : WSABUF ����ü ����
		// lpNumberOfButesRecvd : ���� �Ϸ� �� ���� ����Ʈ�� ���� ������
		// lpFlags : �÷���
		// lpOverlapped : WSAOVERLAPPED ����ü ������
		// lpCompletionRoutine : ���� �Ϸ� �� ȣ��Ǵ� �ݹ��Լ�
		WSARecv(acceptSocket, OUT & wsaBuf, 1, OUT & recvLen, OUT & flags, &overlapped, NULL);
	}

	// ������ �Ϸ���� ���
	t.join();

	// ���� �ݱ�
	closesocket(listenSocket);
	// ����
	WSACleanup();

	return 0;
}