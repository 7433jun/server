#include "pch.h"
#include "IocpEvent.h"

IocpEvent::IocpEvent(EventType type) : eventType(type)
{
	Init();
}

void IocpEvent::Init()
{
	OVERLAPPED::hEvent = NULL;
	OVERLAPPED::Internal = NULL;
	OVERLAPPED::InternalHigh = NULL;
	OVERLAPPED::Offset = NULL;
	OVERLAPPED::OffsetHigh = NULL;
	OVERLAPPED::Pointer = NULL;
}
