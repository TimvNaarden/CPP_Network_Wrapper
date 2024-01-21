#include "Socket/Socket.h"

// Samle Server Configuration
static int Server()
{
	Socket server;
	if (server.Create(IPV4, IPPROTO_UDP, UDP, 54000, SERVER, "127.0.0.1", true))
	{
		return 1;
	}

	server.HandleServerSocket();

	return 0;

}

//Sample Client Configuration
static int Client() {
	Socket client;

	if (client.Create(IPV4, IPPROTO_UDP, UDP, 54000, CLIENT, "127.0.0.1", true))
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
		SOCKADDR_IN dest;
		dest.sin_family = AF_INET;
		dest.sin_port = htons(54000);
		dest.sin_addr.s_addr = inet_addr("127.0.0.1");
		int result = client.SendPacket("Hello World", 0,(SOCKADDR*) & dest);
		
		
		if (result == -1) {
			return 1;
		}
		else if (result == 1)
		{
			CLIENTCMD("Failed to send packet!");
			continue;
		} else if (result == 2)
		{
			CLIENTCMD("Connection Closed");
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
	//Server();
	return Client();

}

