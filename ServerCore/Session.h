#pragma once
#include "IocpObj.h"

class Service;

class Session : public IocpObj
{
	friend class Listener;

private:
	atomic<bool> isConnected = false;
	Service* service = nullptr;
	SOCKET socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr = {};
private:
	RecvEvent recvEvent;
public:
	BYTE recvBuffer[1024] = {};
public:
	Session();
	virtual ~Session();
public:
	SOCKET GetSocket() const { return socket; }
	HANDLE GetHandle() override { return (HANDLE)socket; };
	Service* GetService() const { return service; }
	bool IsConnected() const { return isConnected; }
public:
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
	void SetService(Service* _service) { service = _service; }
private:
	void RegisterRecv();
private:
	void ProcessConnect();
	void ProcessRecv(int numOfBytes);
private:
	void HandleError(int errorCode);
public:
	virtual void OnConnected() {}
	virtual int OnRecv(BYTE* buffer, int len) { return len; }
public:
	void ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred) override;
};

