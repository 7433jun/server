#include "pch.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"


Service::Service(ServiceType type, wstring ip, uint16 port, SessionFactory factory): serviceType(type), sessionFactory(factory)
{
	if (!SocketHelper::StartUp())
		return;

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;

	IN_ADDR address{};
	InetPton(AF_INET, ip.c_str(), &address);
	sockAddr.sin_addr = address;
	sockAddr.sin_port = htons(port);

	// IocpCore 스마트포인터에 할당
	iocpCore = make_shared<IocpCore>();
	sessionCount = 0;
}

Service::~Service()
{
	SocketHelper::CleanUp();
}


shared_ptr<Session> Service::CreateSession()
{
	//session만들어 주고
	shared_ptr<Session> session = sessionFactory();

	session->SetService(shared_from_this());

	if (!iocpCore->Register(session))
		return nullptr;

	return session;
}

//추가
void Service::AddSession(shared_ptr<Session> session)
{
	unique_lock<shared_mutex> lock(rwLock);
	sessionCount++;
	sessions.insert(session);

}

//제거
void Service::RemoveSession(shared_ptr<Session> session)
{
	unique_lock<shared_mutex> lock(rwLock);
	sessions.erase(session);
	sessionCount--;
}

bool Service::ObserveIO(DWORD time)
{
	if (iocpCore != nullptr)
	{
		return iocpCore->ObserveIO(time);
	}

	return false;
}
