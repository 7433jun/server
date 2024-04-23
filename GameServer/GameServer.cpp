#include "pch.h"
#include <ServerService.h>
#include <Session.h>
#include <SendBufferManager.h> 

class ServerSession : public Session
{
public:
	~ServerSession()
	{
		cout << "ServerSession Destroy" << endl;
	}

	virtual void OnConnected() override
	{
	}

	virtual int OnRecv(BYTE* buffer, int len) override
	{
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
		cout << "On DisConnect È£Ãâ" << endl;
	}

};


int main()
{
	printf("==== SERVER ====\n");
	shared_ptr<Service> service = make_shared<ServerService>(L"127.0.0.1", 27015, []() {return make_shared<ServerSession>(); });


	if (!service->Start())
	{
		printf("Server Service Error\n");
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