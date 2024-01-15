#include "Socket/Socket.h"

int main()
{	
	Socket server;
	if((server.Create(IPV4, IPPROTO_TCP, TCP, 54000, SERVER)))
	{
		return 1;
	}

	server.HandleServerSocket();

	return 0;
}