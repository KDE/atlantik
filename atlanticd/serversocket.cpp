#include "serversocket.h"
#include "socket.h"

ServerSocket::ServerSocket(int port, int backlog) : QServerSocket(port, backlog)
{
}

void ServerSocket::newConnection(int socket)
{
	Socket *newSocket = new Socket(this, "socket");
	newSocket->setSocket(socket);

	QString intro;
	intro = "<monopd><server name=\"atlanticd\" version=\"prototype\"/><client clientid=\"-1\" cookie=\"?\"/></monopd>\n";
	newSocket->writeBlock(intro.latin1(), intro.length());
#warning send list of supported games
#warning send list of available games
#warning send list of available commands
}
