#include "serversocket.h"
#include "socket.h"

ServerSocket::ServerSocket(int port, int backlog) : QServerSocket(port, backlog)
{
}

void ServerSocket::newConnection(int socket)
{
	Socket *newSocket = new Socket(this, "socket");
	newSocket->setSocket(socket);
}
