#include "pch.h"
#include "ServerService.h"
#include "Listener.h"


ServerService::ServerService(wstring ip, uint16 port, SessionFactory factory) : Service(ServiceType::SERVER, ip, port, factory)
{
}

bool ServerService::Start()
{
	listener = new Listener;
	return listener->StartAccept(this);
}

