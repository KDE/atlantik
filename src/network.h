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
		void msgInfo(QString);
		void msgStartGame(QString);

		void gamelistUpdate(QString);
		void gamelistEndUpdate(QString);
		void gamelistAdd(QString, QString);
		void gamelistDel(QString);

		void clearPlayerList();
		void addToPlayerList(QString, QString);
		void msgPlayerUpdate(QDomNode);
		void msgEstateUpdate(int, int);
		void setPlayerId(int);
		void setTurn(int);

	private:
		void processMsg(QString);
		void processNode(QDomNode);
		
		QDomDocument msg;
};

#endif
