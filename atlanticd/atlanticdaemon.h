#ifndef	ATLANTIC_ATLANTICDAEMON_H
#define	ATLANTIC_ATLANTICDAEMON_H

class ServerSocket;
class AtlanticCore;

class AtlanticDaemon : public QObject
{
Q_OBJECT

public:
	AtlanticDaemon();

private slots:
	void registerAtMonopigator();

private:
	ServerSocket *m_serverSocket;
	AtlanticCore *m_atlanticCore;

};

#endif
