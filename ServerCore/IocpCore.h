#pragma once
class IocpCore
{
private:
	HANDLE iocpHandle = NULL;
public:
	IocpCore();
	~IocpCore();
public:
	HANDLE GetHandle() const { return iocpHandle; }
public:
	//HANDLE socket, ULONG_PTR key -> IocpObj* iocpObj
	bool Register(class IocpObj* iocpObj);
	bool ObserveIO(DWORD time = INFINITE);
};

