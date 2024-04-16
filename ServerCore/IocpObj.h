#pragma once
#include "IocpEvent.h"
class IocpObj
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred = 0) abstract;
};

