#pragma once
#include "IocpEvent.h" // 빨간줄 때문에 전방선언 IocpEvent쪽 IocpObj 전방선언 꼭
class IocpObj
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred = 0) abstract;
};

