#include "pch.h"
#include <ServerService.h>
#include <Session.h>

class ServerSession : public Session
{
	virtual void OnConnected() override
	{
		cout << "On Connect 호출" << endl;
	}

	virtual int OnRecv(BYTE* buffer, int len) override
	{
		cout << "OnRecv : " << buffer << ", On Recv Len : " << len << endl;

		Send(buffer, len);
		return len;
	}

	virtual void OnSend(int len) override
	{
		cout << "OnSend Len : " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "On DisConnect 호출" << endl;
	}
};


int main()
{
	printf("==== SERVER ====\n");

	Service* service = new ServerService(L"127.0.0.1", 27015, []() {return new ServerSession; });


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

	delete service;

	return 0;
}
