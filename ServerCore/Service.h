#pragma once

class IocpCore;

class Service
{
private:
	SOCKADDR_IN sockAddr = {};
	IocpCore* iocpCore = nullptr;
public:
	Service(wstring ip, u_short port);
	~Service();
public:
	SOCKADDR_IN& GetSockAddr() { return sockAddr; }
	IocpCore* GetIocpCore() const { return iocpCore; }
};

