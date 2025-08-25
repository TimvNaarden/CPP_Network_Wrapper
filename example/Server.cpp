#include "include/TCP/TCPServer.h"
#define _POSIX_C_SOURCE 199309L
#include <time.h>

void sleep_us(unsigned long microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000ul;            // whole seconds
    ts.tv_nsec = (microseconds % 1000000ul) * 1000;  // remainder, in nanoseconds
    nanosleep(&ts, NULL);
}

char Ip[] = "0.0.0.0";

void TCP_Server_Loop(Networking::TCPServer *server, SOCKET sock, SSL *ssl) {
  std::cout << "Client connected" << std::endl;
  sleep(100000);
  char *Message = (char *)"Hello";
  server->Send(sock, Message);
  std::cout << "Message send" << std::endl;
}

int main() {
  Networking::TCPServer MyServer(Networking::IPV4, 7864, Ip, 0);
  MyServer.StartListening(TCP_Server_Loop);
  return 0;
}
