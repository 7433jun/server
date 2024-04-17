#include "pch.h"
#include "ClientService.h"
#include "Session.h"

ClientService::ClientService(wstring ip, uint16 port, SessionFactory factory) : Service(ServiceType::CLIENT, ip, port, factory)
{
}

bool ClientService::Start()
{
    shared_ptr<Session> session = CreateSession();
    return session->Connect();
}
