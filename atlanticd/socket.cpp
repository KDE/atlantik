#include <qtextstream.h>
#include <iostream.h>

#include "socket.moc"

// static QTextStream cout(stdout, IO_WriteOnly);

Socket::Socket(QObject *parent, const char *name) : QSocket(parent, name)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Socket::readData()
{
	while(canReadLine())
		cout << "ignoredLine(" << readLine() << ")" << endl;

	if (bytesAvailable() > (1024 * 32))
		flush();
}
