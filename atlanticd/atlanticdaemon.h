#ifndef	ATLANTIC_ATLANTICDAEMON_H
#define	ATLANTIC_ATLANTICDAEMON_H

class QSocket;

class AtlanticCore;

class ServerSocket;

class AtlanticDaemon : public QObject
{
Q_OBJECT

public:
	AtlanticDaemon();

private slots:
	void monopigatorRegister();
	void monopigatorConnected();

private:
	QSocket *m_monopigatorSocket;
	ServerSocket *m_serverSocket;
	AtlanticCore *m_atlanticCore;

};

#endif
