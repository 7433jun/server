#pragma once
#include "IocpEvent.h"

// IocpObj를 스마트포인터로 레퍼 관리
class IocpObj : public enable_shared_from_this<IocpObj>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void ObserveIO(IocpEvent* iocpEvent, DWORD bytesTransferred = 0) abstract;
};

