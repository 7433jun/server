#include "pch.h"
#include "Listener.h"
#include "ServerService.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"
#include "IocpEvent.h"


Listener::~Listener()
{
    CloseSocket();
}

bool Listener::StartAccept(ServerService* service)
{
	// 처음 생성될떄 받고
	serverService = service;

	socket = SocketHelper::CreateSocket();
	if (socket == INVALID_SOCKET)
		return false;
				  
	if (!SocketHelper::SetReuseAddress(socket, true))
		return false;
	
	if (!SocketHelper::SetLinger(socket, 0, 0))
		return false;

	ULONG_PTR key = 0;
	service->GetIocpCore()->Register(this);


	if (!SocketHelper::Bind(socket, service->GetSockAddr()))
		return false;

	if (!SocketHelper::Listen(socket))
		return false;


	printf("listening...\n");

	AcceptEvent* acceptEvent = new AcceptEvent();
	acceptEvent->iocpObj = this;
	RegisterAccept(acceptEvent);

	return true;

}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	//Session* session = new Session;
	Session* session = serverService->CreateSession();
	session->SetService(serverService);
	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD dwBytes = 0;
	if (!SocketHelper::AcceptEx(socket, session->GetSocket(), session->recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, (LPOVERLAPPED)acceptEvent))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
			RegisterAccept(acceptEvent);
	}
}

void Listener::ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred)
{
	ProcessAccept((AcceptEvent*)iocpEvent);
}


void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	Session* session = acceptEvent->session;
	if (!SocketHelper::SetUpdateAcceptSocket(session->GetSocket(), socket))
	{
		printf("SetUpdateAcceptSocket Error\n");
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddr;
	int sockAddrSize = sizeof(sockAddr);
	//AcceptSocket을 넣어주면 sockAddr에다가 클라의 주소 정보를 넣어줌
	//SOCKET_ERROR라면 문제 있는거니까 에러 처리
	if (getpeername(session->GetSocket(),(SOCKADDR*)&sockAddr, &sockAddrSize) == SOCKET_ERROR)
	{
		//에러 처리
		printf("getpeername Error\n");
		RegisterAccept(acceptEvent);
		return;
	}

	//Session에 클라의 주소를 업데이트
	session->SetSockAddr(sockAddr);
	//연결된거니까 진행해줘라
	session->ProcessConnect();

	//다시 낚시줄을 던진다 == 다른 클라가 접속할수 있도록 등록
	//물고기 == 클라
	RegisterAccept(acceptEvent);
}


void Listener::CloseSocket()
{
	SocketHelper::CloseSocket(socket);
}
