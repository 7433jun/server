#pragma once
#include <functional>
class IocpCore;

enum class ServiceType : uint8
{
	NONE,
	SERVER,
	CLIENT,
};

class IocpCore;
class Session;

using SessionFactory = function<Session* (void)>;

class Service
{
private:
	ServiceType serviceType = ServiceType::NONE;
	SOCKADDR_IN sockAddr = {};
	IocpCore* iocpCore = nullptr;
protected:
	shared_mutex rwLock;
	set<Session*> sessions;
	int sessionCount = 0;
	SessionFactory sessionFactory;
public:
	//Service가 생성될때 SessionFactory도 할당
	Service(ServiceType type, wstring ip, uint16 port, SessionFactory factory);
	virtual ~Service();
public:
	ServiceType GetServiceType() const { return serviceType; }
	SOCKADDR_IN& GetSockAddr() { return sockAddr; }
	IocpCore* GetIocpCore() const { return iocpCore; }
	int GetSessionCount() const { return sessionCount; }
public:
	void SetSessionFactory(SessionFactory func) { sessionFactory = func; }	  //??
public:
	Session* CreateSession();
	void AddSession(Session* session);
	void RemoveSession(Session* session);

public:
	virtual bool Start() abstract;
	bool ObserveIO(DWORD time = INFINITE);
};
