class ServerSocket;

class AtlanticDaemon : public QObject
{
Q_OBJECT

public:
	AtlanticDaemon();

private slots:
	void registerAtMonopigator();

private:
	ServerSocket *m_serverSocket;
};
