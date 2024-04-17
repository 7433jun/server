#pragma once
#include "Service.h"

class Listener;

class ServerService	: public Service
{
private:
	shared_ptr<Listener> listener = nullptr;
public:
	ServerService(wstring ip, uint16 port, SessionFactory factory);
	virtual ~ServerService() {}
public:
	virtual bool Start() override;

};

