#include "pch.h"
#include <Service.h>
#include <Listener.h>

SOCKET listenSocket;
LPFN_ACCEPTEX lpfnAcceptEx = nullptr;
GUID guidAcceptEx = WSAID_ACCEPTEX;

static void AcceptThread(HANDLE iocpHandle)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	WSAOVERLAPPED* overlapped = {};

	while (true)
	{
		printf("Waiting...\n");
		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
		{
			printf("Client connected....\n");

			
		}
	}
}


int main()
{

	printf("==== SERVER ====\n");

	Service service(L"127.0.0.1", 27015);

	Listener listener;

	thread t(AcceptThread, listener.GetHandle());

	listener.StartAccept(service);

	t.join();

	closesocket(listenSocket);

	return 0;
}