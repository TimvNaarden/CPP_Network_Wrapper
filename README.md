# How to use 
Include as a dependency in your premake5.lua file, with include dir Socket 
or copy the Socket class into your project

All of the functions have some explanation in the header file. This is desgined to be a base class, that you can inherent to create your own sockets.
You can find a very basic implementation inside the example.c file

Feel free to use the code, anywhere you'd like. Altough some credit would be nice.

# Notes 
## Testing & UDP
The code is currently not testing and there are still a few bucks.
UDP is currently untested and will not work 

## OpenSSL
You need to create your own SSL certificates inside the folder OpenSSL
This can be done like so:

### Server:
> openssl genpkey -algorithm RSA -out server.key

> openssl req -new -key server.key -out server.csr

> openssl x509 -req -in server.csr -signkey server.key -out server.crt

### Client:
> openssl genpkey -algorithm RSA -out client.key

> openssl req -new -key client.key -out client.csr

> openssl x509 -req -in client.csr -signkey client.key -out client.crt

