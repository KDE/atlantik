#include <qtimer.h>
#include <qsocket.h>
#include <qstring.h>

#include "atlanticdaemon.moc"
#include "serversocket.h"

AtlanticDaemon::AtlanticDaemon()
{
	m_serverSocket = new ServerSocket(1234, 100);

	registerAtMonopigator();
}

void AtlanticDaemon::registerAtMonopigator()
{
	QSocket *socket = new QSocket();
	socket->connectToHost("gator.monopd.net", 80);

	QString get;
	get = "GET /register.php?host=capsi.com&port=1234&version=atlanticd-prototype HTTP/1.1\nHost: gator.monopd.net\n\n";
	socket->writeBlock(get.latin1(), get.length());

#warning close and delete socket
//	socket->close();
//	delete socket;
}
