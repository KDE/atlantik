#include <qsocket.h>

class GameNetwork : public QSocket
{
Q_OBJECT

	public:
		GameNetwork(QObject *parent=0, const char *name=0);

	public slots:
		void slotWrite(const char *);
		void slotRead();

	private:
		void processCmd(QString);
};
