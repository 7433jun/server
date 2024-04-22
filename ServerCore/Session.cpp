#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"
#include "Service.h"
#include "SendBuffer.h"

Session::Session() : recvBuffer(Buffer_SIZE)
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

void Session::Send(shared_ptr<SendBuffer> sendBuffer)
{
	// Lock을 잡고
	unique_lock<shared_mutex> lock(rwLock);

	// sendQueue에 처리할 sendBuffer추가 // 일감 추가
	sendQueue.push(sendBuffer);

	// 내가 처음 send를 하는 스레드 라면
	if (sendRegistered.exchange(true) == false)
	{

		RegisterSend();
	}
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

	// connect Socket일 경우 reuse 걸어준다
	// accept socket같은 경우에는 listener에서 listener socket 값을 업데이트 해줌
	if (!SocketHelper::SetReuseAddress(socket, true))
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
	wsaBuf.buf = (char*)recvBuffer.WritePos();
	wsaBuf.len = recvBuffer.FreeSize();

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

void Session::RegisterSend()
{
	if (!IsConnected())
		return;

	sendEvent.Init();
	sendEvent.iocpObj = shared_from_this();

	int writeSize = 0;
	// sendQueue의 데이터가 남아 있지 않을때까지 돌림
	while (!sendQueue.empty())
	{
		//sendQueue의 앞부분부터 pop시키기 위해서
		shared_ptr<SendBuffer> sendBuffer = sendQueue.front();

		// 얼마나 사용했는지 크기 주기
		writeSize += sendBuffer->WriteSize();

		sendQueue.pop();

		// SendEvent에 들어 있는 값들을 밀어 넣음
		sendEvent.sendBuffers.push_back(sendBuffer);
	}

	// 한꺼번에 데이터를 보내기 위해
	vector<WSABUF> wsaBufs;

	// SendEvent의 sendBuffers크기 만큼 공간 예약
	wsaBufs.reserve(sendEvent.sendBuffers.size());

	for (auto sendBuffer : sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = (char*)sendBuffer->GetBuffer();
		wsaBuf.len = sendBuffer->WriteSize();
		wsaBufs.push_back(wsaBuf);
	}

	DWORD sendLen = 0;
	DWORD flags = 0;

	if (WSASend(socket, wsaBufs.data(), (DWORD)wsaBufs.size(), &sendLen, flags, &sendEvent, nullptr) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent.iocpObj = nullptr;
			sendEvent.sendBuffers.clear();
			sendRegistered.store(false);
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
		ProcessSend(bytesTransferred);
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

	if (recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"On Write overflow");
		return;
	}

	int dataSize = recvBuffer.DataSize();

	int processLen = OnRecv(recvBuffer.ReadPos(), numOfBytes);
	if (processLen < 0 || dataSize < processLen || recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"On Read overflow");
		return;
	}

	recvBuffer.Clear();

	RegisterRecv();

}


void Session::ProcessSend(int numOfBytes)
{

	sendEvent.iocpObj = nullptr;
	sendEvent.sendBuffers.clear();
	

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0 bytes");
	}


	OnSend(numOfBytes);

	unique_lock<shared_mutex> lock(rwLock);
	if (sendQueue.empty())
		sendRegistered.store(false);
	else
		//이어서 쓰기 아직 다 못씀

	sendRegistered.store(false);
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