#include <qtextstream.h>

#include "socket.moc"

// static QTextStream cout(stdout, IO_WriteOnly);

Socket::Socket(QObject *parent = 0, const char *name = 0) : QSocket(parent, name)
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
