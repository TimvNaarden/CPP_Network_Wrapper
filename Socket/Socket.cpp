
#include "Socket.h"


Socket::Socket()
{
	// Init vars
	m_StopListen = 0;
	m_Socket = INVALID_SOCKET;
	m_Type = TCP;
	m_sslctx = nullptr;
	m_ssl = nullptr;
	m_CommunicationType = SERVER;

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

	if (m_sslctx) {
		CleanupSSL();
	}
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
		SERVERCMD("Connection closed");
		return 1;
	}

	SERVERCMD(result);

	delete result;

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
	// Logic to accept connection
	return 1;
}

void Socket::HandleServerSocket()
{
	while (!m_StopListen)
	{
		sockaddr_in addr;
		int addrlen = sizeof(addr);
		SOCKET clientSocket = accept(m_Socket, (struct sockaddr*)&addr, &addrlen);
		if (clientSocket == INVALID_SOCKET) {
			continue;
		}

		if (m_ssl) {
			BIO* sbio = BIO_new_socket(clientSocket, BIO_NOCLOSE);
			SSL_set_bio(m_ssl, sbio, sbio);

			int ret = SSL_connect(m_ssl);
			if (ret <= 0) {
				SERVERCMD("Failed to accept SSL");
				SERVERCMD(SSL_get_error(m_ssl, ret));
				continue;
			}
		}
		std::thread t1 = std::thread(&Socket::handleClient, this, clientSocket);
		t1.detach();
	}
}

int Socket::Create(internetProtocol iprotocol, int protocol, socketType type, int port, communicationType ctype, char* ip, int tls)
{
	m_CommunicationType = ctype;
	m_Type = type;
	m_Socket = socket(iprotocol, type, protocol);
	if (m_Socket == INVALID_SOCKET)
	{
		std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	if (tls) {
		if (EncryptSocket()) {
			CLIENTCMD("Failed to encrypt socket");
			return 1;
		}
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ip);
	service.sin_port = htons(port);

	if (type == UDP || ctype == SERVER)
	{
		if (bind(m_Socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			SERVERCMD("bind() failed.");
			closesocket(m_Socket);
			return 1;
		}


		if (listen(m_Socket, 1) == SOCKET_ERROR)
		{
			SERVERCMD("listen(): Error listening on socket " + WSAGetLastError());
			closesocket(m_Socket);
			return 1;
		}
		return 0;

	}

	if (ctype == CLIENT)
	{
		if (connect(m_Socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			CLIENTCMD("Failed to connect");
			closesocket(m_Socket);
			return 1;
		}

		if (tls) {
			BIO* sbio = BIO_new_socket(m_Socket, BIO_NOCLOSE);
			SSL_set_bio(m_ssl, sbio, sbio);

			int ret = SSL_connect(m_ssl);
			if (ret <= 0) {
				CLIENTCMD(SSL_get_error(m_ssl, ret));
				return 1;

			}
		}
	}
	return 0;
}

int Socket::EncryptSocket()
{

	if (CreateSSLContext()) {
		return 1;
	}

	m_ssl = SSL_new(m_sslctx);
	if (!m_ssl) {
		return 1;
	}

	return 0;
}

int Socket::SendPacket(char* packet, SOCKET dest)
{
	if (!dest) dest = m_Socket;
	if (m_ssl) return SendSSL(packet, dest);
	else return Send(packet, dest);
}

char* Socket::ReceivePacket(SOCKET source)
{
	if (!source) source = m_Socket;
	if (m_ssl) return ReceiveSSL(source);
	else return Receive(source);
}

// Send and receive functions
int Socket::Send(char* packet, SOCKET dest)
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

int Socket::SendSSL(char* packet, SOCKET dest) {
	if (!dest) dest = m_Socket;

	size_t size = strlen(packet);
	std::string stringSize = std::to_string(size + 1);

	int result;

	result = SSL_write(m_ssl, stringSize.c_str(), 64);
	if (result == SOCKET_ERROR)
	{
		if (result == -1)
		{
			std::cerr << "Connection closed." << std::endl;
			return -1;
		}
		std::cerr << "send size failed: " << WSAGetLastError() << std::endl;
		return 1;
	}

	result = SSL_write(m_ssl, packet, size + 1);
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

char* Socket::Receive(SOCKET source)
{
	size_t size = 65536;
	char* stringSize = (char*)malloc(64);
	int result;

	if (stringSize == nullptr) {
		std::cerr << "Failed to allocate memory" << std::endl;
		return nullptr;
	}

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

char* Socket::ReceiveSSL(SOCKET source) {
	size_t size = 65536;
	char* stringSize = (char*)malloc(64);
	int result;

	result = SSL_read(m_ssl, stringSize, 64);
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
	if (stringSize == nullptr) {
		std::cerr << "Could not get stringsize" << std::endl;
		return "Con Closed";
	}
	size = atoi(stringSize);
	char* packet = new char(size);

	result = SSL_read(m_ssl, packet, size);
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

// SSL functions
int Socket::CreateSSLContext()
{
	const SSL_METHOD* method;
	if (m_CommunicationType == CLIENT) method = TLSv1_2_client_method();
	else method = TLSv1_2_server_method();

	m_sslctx = SSL_CTX_new(method);
	if (!m_sslctx) {
		std::cerr << "Unable to create SSL context" << std::endl;
		return 1;
	}
	if (m_CommunicationType == CLIENT) {
		if (SSL_CTX_use_certificate_file(m_sslctx, "OpenSSL/client.crt", SSL_FILETYPE_PEM) <= 0) {
			std::cerr << "Could not load cert" << std::endl;
			return 1;
		}

		if (SSL_CTX_use_PrivateKey_file(m_sslctx, "OpenSSL/client.key", SSL_FILETYPE_PEM) <= 0) {
			std::cerr << "Could not load key" << std::endl;
			return 1;
		}
	}
	else if (m_CommunicationType == SERVER) {
		if (SSL_CTX_use_certificate_file(m_sslctx, "OpenSSL/server.crt", SSL_FILETYPE_PEM) <= 0) {
			std::cerr << "Could not load cert" << std::endl;
			return 1;
		}

		if (SSL_CTX_use_PrivateKey_file(m_sslctx, "OpenSSL/server.key", SSL_FILETYPE_PEM) <= 0) {
			std::cerr << "Could not load key" << std::endl;
			return 1;
		}
	}
	else {
		return 1;
	}
	return 0;
}

void Socket::CleanupSSL()
{
	SSL_shutdown(m_ssl);
	SSL_free(m_ssl);
	SSL_CTX_free(m_sslctx);

	EVP_cleanup();
	ERR_free_strings();
}