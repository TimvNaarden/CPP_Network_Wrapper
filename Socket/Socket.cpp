#include "Socket.h"

Socket::Socket()
{
	// Init vars
	m_stoplisten = 0;
	m_socket = INVALID_SOCKET;
	m_type = TCP;

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
	m_stoplisten = 1; 
	closesocket(m_socket);
	WSACleanup();
	std::terminate();
}

int Socket::Listen()
{
	// Do smh here using recv() and send() functions
	// return 1 to break the loop
	return 1;
}

void Socket::handleClient(SOCKET clientSocket)
{
	std::cout << "Client connected.\n";
	if (!AcceptConnection())
	{
		closesocket(clientSocket);
		return;
	}

	while (true)
	{
		if (Listen())
		{
			break;
		}
	}

}

int Socket::AcceptConnection()
{
	return 0;
}

void Socket::HandleServerSocket()
{
	std::cout << "Waiting for clients...\n";
	while (!m_stoplisten)
	{
		SOCKET clientSocket = accept(m_socket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			continue;
		}
		std::async(std::launch::async, &Socket::handleClient, this, clientSocket);
	}
}

int Socket::Create(internetProtocol iprotocol, IPPROTO protocol, socketType type, int port, communicationType ctype, char* ip)
{
	m_type = type;
	m_socket = socket(iprotocol, type, protocol);
    if (m_socket == INVALID_SOCKET)
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
		if (bind(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			std::cerr << "bind() failed.\n";
			closesocket(m_socket);
			return 1;
		}

		if (listen(m_socket, 1) == SOCKET_ERROR)
		{
			std::cerr << "Error listening on socket.\n";
			closesocket(m_socket);
			return 1;
		}
	}

	if (ctype == CLIENT)
	{
		if (connect(m_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			std::cerr << "Failed to connect.\n";
			closesocket(m_socket);
			return 1;
		}
	}

	return 0;
}	

int Socket::SendPacket(packet* packet)
{
	int result = send(m_socket, (char*)packet, sizeof(packet), 0);
	if (result == SOCKET_ERROR)
	{
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		return 1;
	}
	return 0;
}

int Socket::ReceivePacket(packet* packet)
{
	int result = recv(m_socket, (char*)packet, sizeof(packet), 0);
	if (result == SOCKET_ERROR)
	{
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		return 1;
	}
	return 0;
}
