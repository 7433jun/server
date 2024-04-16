#pragma once
#include "IocpObj.h"

class ServerService;
class AcceptEvent;

class Listener : public IocpObj
{
private:
	// ServerService 추가
	ServerService* serverService = nullptr;
	SOCKET socket = INVALID_SOCKET;
public:
	Listener() = default;
	virtual ~Listener();
public:
	HANDLE GetHandle() override { return (HANDLE)socket; };
	void ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred) override;

public:
	//ServerService로 변경
	bool StartAccept(ServerService* service);
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
	void CloseSocket();

};

