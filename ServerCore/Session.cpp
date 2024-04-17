#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h"

Session::Session()
{
	socket = SocketHelper::CreateSocket();
}

Session::~Session()
{
	SocketHelper::CloseSocket(socket);
}


bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Send(BYTE* buffer, int len)
{
	SendEvent* sendEvent = new SendEvent();

	//스마트 포인터로 변환 : 나와 부모 주소
	sendEvent->iocpObj = shared_from_this();


	sendEvent->sendBuffer.resize(len);


	memcpy(sendEvent->sendBuffer.data(), buffer, len);

	unique_lock<shared_mutex> lock(rwLock);
	RegisterSend(sendEvent);
}

void Session::Disconnect(const WCHAR* cause)
{

	if (isConnected.exchange(false) == false)
		return;

	wprintf(L"Disconnect reason : %ls\n", cause);

	OnDisconnected();
	//스마트 포인터로 변환 : 나의 주소
	GetService()->RemoveSession(GetSession());

	RegisterDisconnect();
}


bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::CLIENT)
		return false;

	if (SocketHelper::BindAny(socket, 0) == false)
		return false;

	connectEvent.Init();
	//스마트 포인터로 변환 : 나와 부모 주소
	connectEvent.iocpObj = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetSockAddr();
	if (SocketHelper::ConnectEx(socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr), nullptr, 0, &numOfBytes, &connectEvent))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != ERROR_IO_PENDING)
		{
			HandleError(errorCode);
			connectEvent.iocpObj = nullptr;
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (!IsConnected())
		return;

	recvEvent.Init();
	//스마트 포인터로 변환 : 나와 부모 주소
	recvEvent.iocpObj = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)recvBuffer;
	wsaBuf.len = sizeof(recvBuffer);

	DWORD recvLen = 0;
	DWORD flags = 0;

	if (WSARecv(socket, &wsaBuf, 1, &recvLen, &flags, &recvEvent, nullptr) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			recvEvent.iocpObj = nullptr;
		}

	}

}

void Session::RegisterSend(SendEvent* sendEvent)
{
	if (!IsConnected())
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->sendBuffer.data();
	wsaBuf.len = sendEvent->sendBuffer.size();

	DWORD sendLen = 0;
	DWORD flags = 0;

	if (WSASend(socket, &wsaBuf, 1, &sendLen, flags, sendEvent, nullptr) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->iocpObj = nullptr;
			delete sendEvent;
		}

	}
}

bool Session::RegisterDisconnect()
{
	disconnectEvent.Init();
	//스마트 포인터로 변환 : 나와 부모 주소
	disconnectEvent.iocpObj = shared_from_this();

	if (SocketHelper::DisconnectEx(socket, &disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			disconnectEvent.iocpObj = nullptr;
			return false;

		}
	}
	return true;
}

void Session::ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred)
{
	switch (iocpEvent->eventType)
	{
	case EventType::CONNECT:
		ProcessConnect();
		break;
	case EventType::RECV:
		ProcessRecv(bytesTransferred);
		break;
	case EventType::SEND:
		ProcessSend((SendEvent*)iocpEvent, bytesTransferred);
		break;
	case EventType::DISCONNECT:
		ProcessDisconnect();
		break;
	default:
		break;
	}
}


void Session::ProcessConnect()
{
	connectEvent.iocpObj = nullptr;

	isConnected.store(true);

	//스마트 포인터로 변환 : 나의 주소
	GetService()->AddSession(GetSession());

	OnConnected();

	RegisterRecv();
}

void Session::ProcessRecv(int numOfBytes)
{
	recvEvent.iocpObj = nullptr;

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0 bytes");
		return;
	}

	OnRecv(recvBuffer, numOfBytes);

	RegisterRecv();

}


void Session::ProcessSend(SendEvent* sendEvent, int numOfBytes)
{

	sendEvent->iocpObj = nullptr;
	delete sendEvent;


	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0 bytes");
	}

	OnSend(numOfBytes);

}


void Session::ProcessDisconnect()
{
	disconnectEvent.iocpObj = nullptr;
}


void Session::HandleError(int errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		printf("Error Code : %d\n", errorCode);
		break;
	}
}