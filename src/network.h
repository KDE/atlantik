#ifndef __KMONOP_NETWORK_H__
#define __KMONOP_NETWORK_H__

#include <qsocket.h>
#include <qdom.h>

class GameNetwork : public QSocket
{
Q_OBJECT

	public:
		GameNetwork(QObject *parent=0, const char *name=0);
		void cmdRoll();
		void cmdBuyEstate();
		void cmdGameStart();
		void cmdEndTurn();
		void cmdName(QString name);
		void cmdTokenConfirmation(int location);
		void cmdEstateMortgage(int estateId);
		void cmdEstateUnmortgage(int estateId);
		void cmdHouseBuy(int estateId);
		void cmdHouseSell(int estateId);
		void cmdGamesList();
		void cmdGameNew();
		void cmdGameJoin(int gameId);
		void cmdChat(QString msg);

	public slots:
		void slotRead();
		
	signals:
		void msgError(QString);
		void msgInfo(QString);
		void msgChat(QString, QString);
		void msgStartGame(QString);

		void gamelistUpdate(QString);
		void gamelistEndUpdate(QString);
		void gamelistAdd(QString, QString);
		void gamelistEdit(QString, QString);
		void gamelistDel(QString);

		void playerlistUpdate(QString);
		void playerlistEndUpdate(QString);
		void playerlistAdd(QString, QString, QString);
		void playerlistEdit(QString, QString, QString);
		void playerlistDel(QString);

		void msgPlayerUpdateName(int, QString);
		void msgPlayerUpdateMoney(int, QString);
		void msgPlayerUpdateLocation(int, int, bool);
		void msgEstateUpdateName(int, QString);
		void msgEstateUpdateOwner(int, int);
		void msgEstateUpdateHouses(int, int);
		void msgEstateUpdateMortgage(int, bool);

		void setPlayerId(int);
		void setTurn(int);

	private:
		void writeData(const char *);
		void processMsg(QString);
		void processNode(QDomNode);
		
		QDomDocument msg;
};

extern GameNetwork *gameNetwork;

#endif
