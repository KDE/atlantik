#include <qtimer.h>
#include <qsocket.h>
#include <qstring.h>

#include <atlantic_core.h>

#include "atlanticdaemon.moc"
#include "serversocket.h"

AtlanticDaemon::AtlanticDaemon()
{
	m_serverSocket = new ServerSocket(1234, 100);
	m_atlanticCore = new AtlanticCore(this, "atlanticCore");

	// Create socket for Monopigator
	m_monopigatorSocket = new QSocket();
	connect(m_monopigatorSocket, SIGNAL(connected()), this, SLOT(monopigatorConnected()));

	// Register server
	monopigatorRegister();
}

void AtlanticDaemon::monopigatorRegister()
{
	m_monopigatorSocket->connectToHost("gator.monopd.net", 80);
}

void AtlanticDaemon::monopigatorConnected()
{
	QString get;
	get = "GET /register.php?host=capsi.com&port=1234&version=atlanticd-prototype HTTP/1.1\nHost: gator.monopd.net\n\n";

	m_monopigatorSocket->writeBlock(get.latin1(), get.length());
	m_monopigatorSocket->close();

	// Monopigator clears old entries, so keep registering every 90s
	QTimer::singleShot(90000, this, SLOT(monopigatorRegister()));
}
