#ifndef __KMONOP_NETWORK_H__
#define __KMONOP_NETWORK_H__

#include <qsocket.h>
#include <qdom.h>

class GameNetwork : public QSocket
{
Q_OBJECT

	public:
		GameNetwork(QObject *parent=0, const char *name=0);
		void writeData(const char *);

	public slots:
		void slotRead();
		
	signals:
		void msgError(QString);
		void msgStartGame(QString);
		void fetchedGameList(QDomNode);
		void fetchedPlayerList(QDomNode);

	private:
		void processMsg(QString);
		void processNode(QDomNode);
		
		QDomDocument msg;
};

#endif
