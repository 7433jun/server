#include "pch.h"
#include "Service.h"

Service::Service(wstring ip, uint16 port)
{
	WSAData wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return;
	}

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	// ip받아서 adress 채운다
	IN_ADDR address;
	InetPton(AF_INET, ip.c_str(), &address);

	sockAddr.sin_addr = address;
	sockAddr.sin_port = htons(port);
}

Service::~Service()
{
	WSACleanup();
}