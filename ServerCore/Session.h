#pragma once
#include "IocpObj.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"

class Service;

//constexpr auto BUUFER_SIZE = 0x10000; //64kb

class Session : public IocpObj
{
	friend class Listener;
	enum { BUFFER_SIZE = 0x10000 };

private:
	shared_mutex rwLock;
	atomic<bool> isConnected = false;
	shared_ptr<Service> service = nullptr;
	SOCKET socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr = {};
private:
	ConnectEvent connectEvent;
	//SendEvent 추가
	SendEvent sendEvent;
	RecvEvent recvEvent;
	DisconnectEvent disconnectEvent;
public:
	//SendBuffer queue 추가
	queue<shared_ptr<SendBuffer>> sendQueue;
	//atomic으로 send를 등록했는지 안했는지
	atomic<bool> sendRegistered = false;
	RecvBuffer recvBuffer;

public:
	Session();
	virtual ~Session();
public:
	SOCKET GetSocket() const { return socket; }
	HANDLE GetHandle() override { return (HANDLE)socket; };
	shared_ptr<Service>  GetService() const { return service; }
	shared_ptr<Session> GetSession() { return static_pointer_cast<Session>(shared_from_this()); }
public:
	bool IsConnected() const { return isConnected; }
public:
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
	void SetService(shared_ptr<Service>  _service) { service = _service; }
private:
	bool RegisterConnect();
	//SendEvent 들구 있을꺼라
	void RegisterSend();
	void RegisterRecv();
	bool RegisterDisconnect();
private:
	void ProcessConnect();
	//SendEvent 들구 있을꺼라
	void ProcessSend(int numOfBytes);
	void ProcessRecv(int numOfBytes);
	void ProcessDisconnect();
private:
	void HandleError(int errorCode);
public:
	virtual void OnConnected() {}
	virtual void OnSend(int len) {}
	virtual int OnRecv(BYTE* buffer, int len) { return len; }
	virtual void OnDisconnected() {}
public:
	bool Connect();
	//SendBuffer로 수정
	void Send(shared_ptr<SendBuffer> sendBuffer);
	void Disconnect(const WCHAR* cause);
public:
	void ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred) override;
};