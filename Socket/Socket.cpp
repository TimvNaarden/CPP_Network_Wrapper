#include "Socket.h"



Socket::Socket()
{
	// Init vars
	m_StopListen = 0;
	m_Socket = INVALID_SOCKET;
	m_Type = TCP;
	m_sslctx = nullptr;

	// Init winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed.\n";
		return;
	}
}

Socket::~Socket()
{
	m_StopListen = 1;
	closesocket(m_Socket);
	WSACleanup();
}

int Socket::Listen(SOCKET clientSocket)
{
	// Default implementation prints the received packet
	char* result = ReceivePacket(clientSocket);
	if (result == nullptr)
	{
		return 0;
	}
	else if (result == "Con Closed")
	{
		return 1;
	}

	std::cout << result << std::endl;
	free(result);

	// return 1 to break the loop
	return 0;
}

void Socket::handleClient(SOCKET clientSocket)
{
	if (!AcceptConnection())
	{
		closesocket(clientSocket);
		return;
	}

	while (true)
	{
		if (Listen(clientSocket))
		{
			break;

		}
	}
	closesocket(clientSocket);

}

int Socket::AcceptConnection()
{
	return 1;
}

void Socket::HandleServerSocket()
{
	while (!m_StopListen)
	{
		SOCKET clientSocket = accept(m_Socket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			continue;
		}
		handleClient(clientSocket);
		std::async(std::launch::async, &Socket::handleClient, this, clientSocket);
	}
}

int Socket::Create(internetProtocol iprotocol, int protocol, socketType type, int port, communicationType ctype, char* ip, int tls)
{
	m_Type = type;
	m_Socket = socket(iprotocol, type, protocol);
	if (m_Socket == INVALID_SOCKET)
	{
		std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ip);
	service.sin_port = htons(port);

	if (type == UDP || ctype == SERVER)
	{
		if (bind(m_Socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			std::cerr << "bind() failed.\n";
			closesocket(m_Socket);
			return 1;
		}

		if (listen(m_Socket, 1) == SOCKET_ERROR)
		{
			std::cerr << "Error listening on socket.\n";
			closesocket(m_Socket);
			return 1;
		}
	}

	if (ctype == CLIENT)
	{
		if (connect(m_Socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			std::cerr << "Failed to connect.\n";
			closesocket(m_Socket);
			return 1;
		}
	}

	if (tls)
	{
		return EncryptSocket();
	}

	return 0;
}

int Socket::EncryptSocket()
{
	const SSL_METHOD* method;

	method = TLS_server_method();

	m_sslctx = SSL_CTX_new(method);
	if (!m_sslctx) {
		std::cerr << "Unable to create SSL context" << std::endl;
		return 1;
	}

	/* Set the key and cert */
	if (SSL_CTX_use_certificate_file(m_sslctx, "OpenSSL/cert.pem", SSL_FILETYPE_PEM) <= 0) {
		std::cerr << "Could not load cert" << std::endl;
		return 1;
	}

	if (SSL_CTX_use_PrivateKey_file(m_sslctx, "OpenSSL/key.pem", SSL_FILETYPE_PEM) <= 0) {
		std::cerr << "Could not load key" << std::endl;
		return 1;
	}
	return 0;
}		

int Socket::SendPacket(char* packet, SOCKET dest)
{
	if (!dest) dest = m_Socket;

	size_t size = strlen(packet);
	std::string stringSize = std::to_string(size + 1);

	int result;

	result = send(dest, stringSize.c_str(), 64, 0);
	if (result == SOCKET_ERROR)
	{
		if (result == -1)
		{
			std::cerr << "Connection closed." << std::endl;
			return 2;
		}
		std::cerr << "send size failed: " << WSAGetLastError() << std::endl;
		return 1;
	}

	result = send(dest, packet, size + 1, 0);
	if (result == SOCKET_ERROR)
	{
		if (result == -1)
		{
			std::cerr << "Connection closed." << std::endl;
			return 2;
		}
		std::cerr << "send data failed: " << WSAGetLastError() << std::endl;
		return 1;
	}



	return 0;
}

char* Socket::ReceivePacket(SOCKET source)
{
	size_t size = 65536;
	char* stringSize = (char*)malloc(64);

	int result;

	result = recv(source, (char*)stringSize, 64, 0);
	if (result == SOCKET_ERROR)
	{
		if (result == -1)
		{
			std::cerr << "Connection closed." << std::endl;
			return "Con Closed";
		}
		std::cerr << "recv size failed: " << WSAGetLastError() << std::endl;
		return nullptr;
	}
	size = atoi(stringSize);
	char* packet = new char[size];

	result = recv(source, packet, size, 0);
	if (result == SOCKET_ERROR)
	{
		if (result == -1)
		{
			std::cerr << "Connection closed." << std::endl;
			return "Con Closed";
		}
		std::cerr << "recv data failed: " << WSAGetLastError() << std::endl;
		return nullptr;
	}
	// Free memory
	free(stringSize);

	if (packet[0] == -3) {
		std::cerr << "Connection closed." << std::endl;
		return "Con Closed";
	}
	return packet;
}
