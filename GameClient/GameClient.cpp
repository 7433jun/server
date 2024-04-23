#pragma once
#include "pch.h"
#include <ClientService.h>
#include <Session.h>
#include <SendBufferManager.h>

char sendData[] = "Hello This is Client";

class ClientSession : public Session
{
public:
	~ClientSession()
	{
		cout << "Client Session Destroy" << endl;
	}

	virtual void OnConnected() override
	{
		shared_ptr<SendBuffer> sendBuffer = SendBufferManager::Get().Open(4096);
		memcpy(sendBuffer->GetBuffer(), sendData, sizeof(sendData));
		if (sendBuffer->Close(sizeof(sendData)))
		{
			Send(sendBuffer);
		}
	}

	virtual int OnRecv(BYTE* buffer, int len) override
	{
		this_thread::sleep_for(0.01s);

		shared_ptr<SendBuffer> sendBuffer = SendBufferManager::Get().Open(4096);
		memcpy(sendBuffer->GetBuffer(), buffer, len);
		if (sendBuffer->Close(len))
		{
			Send(sendBuffer);
		}

		return len;
	}

	virtual void OnSend(int len) override
	{
	}

	virtual void OnDisconnected() override
	{
		cout << "On disconnected" << endl;
	}



};


int main()
{
	printf("==== CLIENT ====\n");

	shared_ptr<Service> service = make_shared<ClientService>(L"127.0.0.1", 27015, []() {return make_shared<ClientSession>(); });

	//1000명정도 접속 시작
	for (int i = 0; i < 1000; i++)
	{
		if (!service->Start())
		{
			printf("Server Service Error\n");
			return 1;
		}
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