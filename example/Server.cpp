#include "include/TCP/TCPServer.h"
char Ip[] = "0.0.0.0";

void TCP_Server_Loop(Networking::TCPServer *server, SOCKET sock, SSL *ssl) {
  std::cout << "Client connected" << std::endl;

  char *Message = (char *)"Hello";
  server->Send(sock, Message);
  std::cout << "Message send" << std::endl;
}

int main() {
  Networking::TCPServer MyServer(Networking::IPV4, 7864, Ip, 0);
  MyServer.StartListening(TCP_Server_Loop);
  return 0;
}
