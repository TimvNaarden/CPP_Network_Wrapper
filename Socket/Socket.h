#pragma once

#include <iostream>
#include <future>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")


#include "openssl/ssl.h"
#include "openssl/err.h"
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

// Macro's 
#define SERVERCMD(x) std::cout << "Server: " << x << std::endl;
#define CLIENTCMD(x) std::cout << "Client: " << x << std::endl;

#define _WINSOCK_DEPRECATED_NO_WARNINGS

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

		// Creates a socket
		// Returns 0 if successful
		// After creating call the appropriate function to handle the socket
		virtual int Create(internetProtocol iprotocol,				// IPV4 or IPV6
										socketType type,			// TCP or UDP
										communicationType ctype,	// Client or Server
										int port = 0,				// If using TCP or UDP server specify port 
										char* ip = "127.0.0.1",		// Specify ip for servers to bind or tcp client to connect to 
										int SSLEncryption = false	// Enable SSL encryption	
							);

		// This function will wait for incoming connections
		// It will be called in a loop until m_stoplisten is set to 1
		// Default implementation will accept incoming clients and call the handleClient() function in a new thread
		virtual void HandleServerSocket();

		// Optional function to accept connection
		// Will return 1 if not altered 
		// To refuse the connection return 0
		// TCP server only
		virtual int AcceptConnection();

		// Send a packet
		// Do not specify a destination if using TCP client or server
		virtual int SendPacket(char* packet, SOCKET dest = 0, SOCKADDR* destaddr = {});

		// Receive a packet
		// Delete the packet after
		virtual char* ReceivePacket(SOCKET source = 0);
		

	// These functions here, are not ment to be changed
	private:

		// Determes what to do with the client socket after accepting connection 
		// Calls AcceptConnection() function to check if connection should be intialized
		// Calls listen() function in a loop until it returns 1 to break the loop
		virtual void handleClient(SOCKET clientSocket);

		// After the server socket has an connection with a client socket
		// this function wil run in a loop until it returns 1
		// To break the loop return 1
		virtual int Listen(SOCKET clientSocket);

		int m_StopListen = 1;
		socketType m_Type;
		SOCKET m_Socket;
		communicationType m_CommunicationType;

		// Send and receive functions
		virtual int Send(char* packet, SOCKET dest);
		virtual int SendUDP(char* packet, SOCKADDR* destaddr);
		virtual int SendSSL(char* packet, SOCKET dest);

		virtual char* Receive(SOCKET source);
		virtual char* ReceiveSSL(SOCKET source);
		virtual char* ReceiveUDP(SOCKET source);
		
		// SSL functions
		int EncryptSocket();
		int CreateSSLContext();
		void CleanupSSL();


		// SSL variables
		SSL_CTX* m_sslctx;
		SSL* m_ssl;
};

