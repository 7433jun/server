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
	//bool·Î ¹Ù²Þ
	bool Register(class IocpObj* iocpObj);
	bool ObserveIO(DWORD time = INFINITE);
};

