#include <qserversocket.h>

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H
class ServerSocket : public QServerSocket
{
public:
	ServerSocket(int port, int backlog);
	void newConnection(int socket);
};
#endif
