#include "Socket/Socket.h"

// Samle Server Configuration
static int Server()
{
	Socket server;
	if (server.Create(IPV4, TCP, SERVER, 54000))
	{
		return 1;
	}

	server.HandleServerSocket();

	return 0;

}

//Sample Client Configuration
static int Client() {
	Socket client;

	if (client.Create(IPV4, TCP, CLIENT, 54000))
	{
		CLIENTCMD("Failed to create socket!");
		return 1;
	}
	else
	{
		CLIENTCMD("Connected");
	}

	while (true) 
	{
		int result = client.SendPacket("Hello World");
		
		if (result == -1) {
			CLIENTCMD("Connection Closed");
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
}
int main()
{
	std::thread serverThread(Server);
	serverThread.detach();
	return Client();

}

