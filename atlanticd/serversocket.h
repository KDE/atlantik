#include <qserversocket.h>

class ServerSocket : public QServerSocket
{
public:
	ServerSocket(int port, int backlog);
	void newConnection(int socket);
};

