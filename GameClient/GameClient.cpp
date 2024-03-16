#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

int main()
{
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
		printf("socket function failed with error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Connect to Server\n");

	char recvBuffer[512];
	int recvLen = recv(connectSocket, recvBuffer, sizeof(recvBuffer), 0);

	if (recvLen <= 0)
	{
		printf("Recv Error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	printf("Recv buffer Data : %s\n", recvBuffer);
	printf("Recv buffer Length : %d butes\n", recvLen);

	while (true)
	{
		char sendBuffer[] = "Hello this is Client!";

		if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			printf("Send Error %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			continue;
		}

		printf("Send Data : %s\n", sendBuffer);

		Sleep(1000);

		if (GetAsyncKeyState(VK_RETURN))
		{
			shutdown(connectSocket, SD_BOTH);
			break;
		}
	}

	closesocket(connectSocket);
	WSACleanup();
}