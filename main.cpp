#include "Socket/Socket.h"

int main()
{
	Socket client;
	if (client.Create(IPV4, IPPROTO_TCP, TCP, 54000, SERVER, "127.0.0.1", true))
	{
		std::cout << "failed to create socket" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "connected" << std::endl;
	}

	while (true) {
		int result = client.SendPacket("Hello World");
		if (result == 2) {
			std::cout << "Socket closed" << std::endl;
			return 1;
		}
		else if (result == 1)
		{
			std::cout << "failed to send packet" << std::endl;
			continue;
		}
		else
		{
			std::cout << "packet sent" << std::endl;
		}
	}


	return 0;
}