#ifndef __KMONOP_NETWORK_H__
#define __KMONOP_NETWORK_H__

#include <qsocket.h>
#include <qdom.h>

class GameNetwork : public QSocket
{
Q_OBJECT

	public:
		GameNetwork(QObject *parent=0, const char *name=0);

	public slots:
		void slotWrite(const char *);
		void slotRead();
		
	signals:
		void fetchedGameList(QDomNode);

	private:
		void processMsg(QString);
		void processNode(QDomNode);
		
		QDomDocument msg;
};

#endif
