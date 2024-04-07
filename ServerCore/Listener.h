#pragma once

class Listener
{
private:
	SOCKET socket = INVALID_SOCKET;
	//HANDLE iocpHandle = NULL;
	//LPFN_ACCEPTEX lpfnAcceptEx = NULL;
	//GUID guidAcceptEx = WSAID_ACCEPTEX;

public:
	Listener() = default;
	~Listener();
public:
	bool StartAccept(class Service& service);
	void CloseSocket();
};

