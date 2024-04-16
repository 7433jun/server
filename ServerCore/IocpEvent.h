#pragma once

enum class EventType : uint8
{
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND,

};

class IocpEvent : public OVERLAPPED
{
public:
	EventType eventType;
	class IocpObj* iocpObj;
public:
	IocpEvent(EventType type);
public:
	void Init();
};

//ConnectEvent 추가
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::CONNECT) {}
};

//DisconnectEvent 추가
class DisonnectEvent : public IocpEvent
{
public:
	DisonnectEvent() : IocpEvent(EventType::DISCONNECT) {}
};

class AcceptEvent : public IocpEvent
{
public:
	class Session* session = nullptr;
public:
	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};


class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}
};

class SendEvent : public IocpEvent
{
public:
	vector<BYTE> sendBuffer;
public:
	SendEvent() : IocpEvent(EventType::SEND) {}
};