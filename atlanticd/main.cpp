#include <qapplication.h>

#include "serversocket.h"

int main(int argc, char *argv[])
{
	ServerSocket *serverSocket = new ServerSocket(1234, 100);

	QApplication qapplication(argc, argv);
	qapplication.exec();
}
