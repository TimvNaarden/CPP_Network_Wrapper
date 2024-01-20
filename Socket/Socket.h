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
		// If making a client socket, specify ip address
		// After creating call the appropriate function to handle the socket
		// Last input true for TLS
		virtual int Create(internetProtocol iprotocol, int protocol, socketType type, int port, communicationType ctype, char* ip = "127.0.0.1", int tls = 0);

		// This function will wait for incoming connections
		// It will be called in a loop until m_stoplisten is set to 1
		// Default implementation will accept incoming clients and call the handleClient() function in a new thread
		virtual void HandleServerSocket();

		// Optional function to accept connection
		// Will return 0 if not altered 
		// To refuse the connection return 1
		virtual int AcceptConnection();

		// Send a packet
		// Do not specify a destination if using TCP client or server
		virtual int SendPacket(char* packet, SOCKET dest = 0);

		// Receive a packet
		virtual char* ReceivePacket(SOCKET source);
		

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
		virtual int SendSSL(char* packet, SOCKET dest);

		virtual char* Receive(SOCKET source);
		virtual char* ReceiveSSL(SOCKET source);

		// SSL functions
		int EncryptSocket();
		int CreateSSLContext();
		void CleanupSSL();


		// SSL variables
		SSL_CTX* m_sslctx;
		SSL* m_ssl;
};

