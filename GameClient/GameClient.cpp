#pragma once
#include "pch.h"
#include <ClientService.h>
#include <Session.h>

char sendBuffer[] = "Hello this is Client";

class ClientSession : public Session
{
public:
	~ClientSession()
	{
		cout << "Client Session Destroy" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connect to Server" << endl;
		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
	}

	virtual int OnRecv(BYTE* buffer, int len) override
	{
		cout << "OnRecv : " << (char*)buffer << "Len : " << len << endl;

		this_thread::sleep_for(1s);

		Send(buffer, len);

		return len;
	}

	virtual void OnSend(int len) override
	{
		cout << "On Send Len : " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "On disconnect" << endl;
	}
};


int main()
{
	printf("==== CLIENT ====\n");

	shared_ptr<Service> service = make_shared<ClientService>(L"127.0.0.1", 27015, []() {return make_shared<ClientSession>(); });


	if (!service->Start())
	{
		printf("Client Service Error\n");
		return 1;
	}

	thread t([=]()
		{
			while (true)
			{
				service->ObserveIO();
			}
		}
	);


	t.join();

	return 0;
}
