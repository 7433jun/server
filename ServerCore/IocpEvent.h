#pragma once

enum class EventType : uint8
{
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND,

};

class IocpObj;
class Session;

class IocpEvent : public OVERLAPPED
{
public:
	EventType eventType;
	shared_ptr<IocpObj> iocpObj;
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
class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::DISCONNECT) {}
};

class AcceptEvent : public IocpEvent
{
public:
	shared_ptr<Session> session = nullptr;
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