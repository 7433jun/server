#pragma once
#include "IocpObj.h"

class Service;

class Session : public IocpObj
{
	friend class Listener;

private:
	shared_mutex rwLock;
	//연결관리
	atomic<bool> isConnected = false;
	//서비스 들고 있을 용도로
	Service* service = nullptr;
	SOCKET socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr = {};
private:
	ConnectEvent connectEvent;
	RecvEvent recvEvent;
public:
	BYTE recvBuffer[1024] = {};
public:
	Session();
	virtual ~Session();
public:
	SOCKET GetSocket() const { return socket; }
	HANDLE GetHandle() override { return (HANDLE)socket; };
	//Get
	Service* GetService() const { return service; }
	bool IsConnected() const { return isConnected; }
public:
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
	//Set
	void SetService(Service* _service) { service = _service; }
private:
	bool RegisterConnect();
	void RegisterSend(SendEvent* sendEvent);
	void RegisterRecv();
private:
	void ProcessConnect();
	void ProcessSend(SendEvent* sendEvent, int numOfBytes);
	void ProcessRecv(int numOfBytes);
private:
	//에러 관리
	void HandleError(int errorCode);
public:
	//자식이 상속받아서 사용할 용도로
	virtual void OnConnected() {}
	virtual void OnSend(int len) {}
	virtual int OnRecv(BYTE* buffer, int len) { return len; }
	virtual void OnDisconnected() {}
public:
	bool Connect();
	void Send(BYTE* buffer, int len);
	void Disconnect(const WCHAR* cause);
public:
	void ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred) override;
};

