#include <qsocket.h>

class Socket : public QSocket
{
Q_OBJECT

public:
	Socket(QObject *parent = 0, const char *name = 0);

private slots:
	void readData();
};

