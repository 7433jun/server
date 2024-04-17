#include "pch.h"
#include "ServerService.h"
#include "Listener.h"


ServerService::ServerService(wstring ip, uint16 port, SessionFactory factory) : Service(ServiceType::SERVER, ip, port, factory)
{
}

bool ServerService::Start()
{
	// ����Ʈ �����Ϳ� �Ҵ�
	listener = make_shared<Listener>();
	return listener->StartAccept(static_pointer_cast<ServerService>(shared_from_this()));
}

