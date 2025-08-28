#include "include/TCP/TCPClient.h"
char Ip[] = "127.0.0.1";

int main() {
  Networking::TCPClient MyClient(Networking::IPV4, 7865, Ip, 0);
  char *Message;
  int res = MyClient.Receive(Message);
  std::cout << Message << std::endl;
  return 0;
}
