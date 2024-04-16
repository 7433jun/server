#pragma once
#include "Service.h"

class ClientService : public Service
{
public:
	ClientService(wstring ip, uint16 port, SessionFactory factory);
	virtual ~ClientService() {}
public:
	virtual bool Start() override;
};

