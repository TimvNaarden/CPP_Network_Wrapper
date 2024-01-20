#include "Socket/Socket.h"

int Sample()
{
	Socket client;

	if (client.Create(IPV4, IPPROTO_TCP, TCP, 54000, CLIENT, "127.0.0.1", true))
	{
		CLIENTCMD("Failed to create socket!");
		return 1;
	}
	else
	{
		std::cout << "Client: Connected!" << std::endl;
	}

	while (true) {
		int result = client.SendPacket("Hello World");
		if (result == -1) {
			return 1;
		}
		else if (result == 1)
		{
			CLIENTCMD("Failed to send packet!");
			continue;
		}
		else
		{
			CLIENTCMD("Packet sent!");
		}
	}

	return 0;

}

// Samle Server Configuration
int Server()
{
	Socket server;
	if (server.Create(IPV4, IPPROTO_TCP, TCP, 54000, SERVER, "127.0.0.1", true))
	{
		return 1;
	}

	// Default implementation will accept incoming clients and call the handleClient() function in a new thread
	// Which will than wait for packets and print them to the console, like this: Server: {Mesag}
	server.HandleServerSocket();

	return 0;
}