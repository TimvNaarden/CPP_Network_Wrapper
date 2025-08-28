# How to use

Include as a dependency in your premake5.lua file
or copy the contents of the include folder directly into your project

All of the functions have some explanation in the header file. This is desgined to be a base class, that you can inherent to create your own sockets.
Inside the folder example is a very simple example that just sends a message from the server to the client.

Feel free to use the code, anywhere you'd like. Altough some credit would be nice.

## TCPClient

### Constructor

```cpp
TCPClient(iProtocol iProt, UINT16 port, char *ip, int SSL = 0);
```

Since TCP is connection based you need to specify the Protocol(Networking::IPV4 or Networking::IPV6) the IP-Address and the desired port. SSL is an extra option if you want to enable encryption(default is 0, off).

### Sending Data

```cpp
int Send(const char *data, int length = 0);
```

Since there is already an active connection with a server, you only need to specify a pointer to the data and optionally the size, for strings this is not necesarry but for other types of variables it is.<br>
NOTE: The server needs to be actively waiting for a packet by calling the receive function.<br>
Returns 0 on Succes

### Receiving Data

```cpp
int Receive(char *&data);
```

The only argument this function requires is a char pointer, it should be a uninitialized pointer since it gets reallocated.
This function waits UNTIL there is a message.<br>
NOTE: Don forget to call `free(data)` after you are done with the result<br>
Returns 0 on Succes

## TCPServer

### Constructor

```cpp
TCPServer(iProtocol iProt, UINT16 port, char *ip, int SSL = 0);
```

Since TCP is connection based you need to specify the Protocol(Networking::IPV4 or Networking::IPV6) the IP-Address and the desired port. SSL is an extra option if you want to enable encryption(default is 0, off).

### Start listening

```cpp
  void StartListening(void (*callback)(TCPServer *, SOCKET, SSL *));
```

With this function you can decide what to do with every client socket that connects <br>
It expects a function in the form off `void Listen_callback(TCPServer this, SOCKET client, SSL c_SSL)`
If a client connects you can do the logic with send or receive for communication with the two, see the examples.
This function does start a while true loop, just waiting for clients. If you desire other use cases, just alter the source code.

### Sending Data

```cpp
int Send(const char *data, int length = 0);
```

Since there is already an active connection with a server, you only need to specify a pointer to the data and optionally the size, for strings this is not necesarry but for other types of variables it is.<br>
NOTE: The server needs to be actively waiting for a packet by calling the receive function.<br>
Returns 0 on Succes

### Receiving Data

```cpp
int Receive(char *&data);
```

The only argument this function requires is a char pointer, it should be a uninitialized pointer since it gets reallocated.
This function waits UNTIL there is a message.<br>
NOTE: Don forget to call `free(data)` after you are done with the result<br>
Returns 0 on Succes

## UDPClient

### Constructor

```cpp
  UDPClient(iProtocol iProt);
```

You only need to specify the protocol(Networking::IPV4 or Networking::IPV6) which the socket is going to be using for sending and receiving data.

### Sending data

```cpp
  int Send(const char *data, char *ip, UINT16 port, int length = 0);
```

Since there is no active connection you need to specify the target port and IP-Address. Specifing the length is necesarry when sendign something other than a string.<br>
NOTE: The server needs to be actively waiting for a packet by calling the receive function.<br>
Returns 0 on Succes

### Receiving data

```cpp
  int Receive(char *&data, sockaddr_in *ServerAddress);
```

Since it is using the UTP protocol you will also need to specify a source address. For the data param it requires a char pointer, which should be uninitialized since it gets reallocated.
This function waits UNTIL there is a message.<br>
NOTE: Don forget to call `free(data)` after you are done with the result<br>
Returns 0 on Succes

## UDPServer

## Constructor

```cpp
  UDPServer(iProtocol iProt, UINT16 port, char *ip);
```

You need to specify the IP-Address, port and protocol(Networking::IPV4 or Networking::IPV6) the server will be listening on.

### Sending data

```cpp
  int Send(const char *data, char *ip, UINT16 port, int length = 0);
```

Since there is no active connection you need to specify the target port and IP-Address. Specifing the length is necesarry when sendign something other than a string.<br>
NOTE: The server needs to be actively waiting for a packet by calling the receive function.<br>
Returns 0 on Succes

### Receiving data

```cpp
  int Receive(char *&data, sockaddr_in *ServerAddress);
```

Since it is using the UTP protocol you will also need to specify a source address. For the data param it requires a char pointer, which should be uninitialized since it gets reallocated.
This function waits UNTIL there is a message.<br>
NOTE: Don forget to call `free(data)` after you are done with the result<br>
Returns 0 on Succes

## Errors

When there is a fault with receiving data or a connection doesn't get established, it throws an std error. Just use a `try{} catch(std::exception e){}` block.
