#include "Socket/Socket.h"

int main()
{	
	Socket client;
	client.Create(IPV4, IPPROTO_TCP, TCP, 54000, CLIENT);

	return 0;
}