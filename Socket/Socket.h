#pragma once
#include <iostream>
#include <future>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

namespace Networking
{
	class Socket;
	enum socketType;
	enum internetProtocol;
	enum communicationType;
}

enum socketType
{
	TCP = SOCK_STREAM,
	UDP = SOCK_DGRAM,
};

enum communicationType
{
	CLIENT = 0,
	SERVER = 1
};

enum internetProtocol
{
	IPV4 = AF_INET,
	IPV6 = AF_INET6
};

class Socket
{
	public:
		Socket();
		~Socket();

		// Will be send and received modify the packet struct to your needs
		virtual struct packet
		{
			int id;
			int size;
			char* data;
		};

		// Creates a socket
		// Returns 0 if successful
		// If making a client socket, specify ip address
		// After creating call the appropriate function to handle the socket
		int Create(internetProtocol iprotocol, IPPROTO protocol, socketType type, int port, communicationType ctype, char* ip = "127.0.0.1");

		// This function will wait for incoming connections
		// It will be called in a loop until m_stoplisten is set to 1
		// Default implementation will accept incoming clients and call the handleClient() function in a new thread
		virtual void HandleServerSocket();


		virtual int SendPacket(packet* packet);
		virtual int ReceivePacket(packet* packet);



	private:

		// Determes what to do with the client socket after accepting connection 
		// Calls AcceptConnection() function to check if connection should be intialized
		// Calls listen() function in a loop until it returns 1 to break the loop
		virtual void handleClient(SOCKET clientSocket);

		// Optional function to accept connection
		// Will return 0 if not altered 
		// To refuse the connection return 1
		virtual int AcceptConnection();

		// After the server socket has an connection with a client socket
		// this function wil run in a loop until it returns 1
		// To break the loop return 1
		virtual int Listen();
		SOCKET m_socket;
		int m_stoplisten;
		socketType m_type;
};

