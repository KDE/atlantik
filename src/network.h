#ifndef ATLANTIK_NETWORK_H
#define ATLANTIK_NETWORK_H

#include <qsocket.h>
#include <qdom.h>
#include <qptrlist.h>
#include <qmap.h>

class AtlanticCore;
class Atlantik;

class Trade;
class Estate;
class Player;

class GameNetwork : public QSocket
{
Q_OBJECT

public:
	GameNetwork(AtlanticCore *atlanticCore, Atlantik *parent=0, const char *name=0);
	void cmdName(QString name);
	void cmdGamesList();
	void cmdTradeAccept(int tradeId);
	void cmdTradeReject(int tradeId);
	void cmdChat(QString msg);

	Player *self();

private slots:
	void roll();
	void endTurn();
	void newGame(const QString &gameType);
	void joinGame(int gameId);
	void startGame();
	void buyEstate();
	void estateToggleMortgage(Estate *estate);
	void estateHouseBuy(Estate *estate);
	void estateHouseSell(Estate *estate);
	void jailCard();
	void jailPay();
	void jailRoll();
	void newTrade(Player *player);
	void tokenConfirmation(Estate *);
	void tradeUpdateEstate(Trade *trade, Estate *estate, Player *player);
	void tradeUpdateMoney(Trade *trade, Player *pFrom, Player *pTo, unsigned int money);

public slots:
	void serverConnect(const QString host, int port);
	void slotRead();
		
signals:
	void msgError(QString);
	void msgChat(QString, QString);
	void msgStartGame(QString);

	void displayCard(QString, QString);

	void gameListClear();
	void gamelistEndUpdate(QString);
	void gameListAdd(QString gameId, QString gameType, QString description, QString players);
	void gameListEdit(QString gameId, QString gameType, QString description, QString players);
	void gameListDel(QString gameId);
	void joinedGame();
	void initGame();
	void startedGame();

	void playerListClear();
	void playerListEndUpdate(QString);
	void playerListAdd(QString, QString, QString);
	void playerListEdit(QString, QString, QString);
	void playerListDel(QString);

	void tradeUpdateActor(int tradeId, int playerId);

	/**
	 * Information from server whether a player accepts the trade as is.
	 *
	 * @param tradeId  Unique trade identifier
	 * @param playerId Unique player identifier
	 * @param accept   Whether the player accepts or not
	 */
	void msgTradeUpdatePlayerAccept(int tradeId, int playerId, bool accept);

	/**
	 * The trade has been accepted by all players!
	 *
	 * @param tradeId  Unique trade identifier
	 */
	void msgTradeUpdateAccepted(int tradeId);

	/**
	 * The trade has been completed. Emitted after all necessary estate and
	 * player updates are processed.
	 *
	 * @param tradeId  Unique trade identifier
	 */
	void msgTradeUpdateCompleted(int tradeId);

	/**
	 * One of the players rejected the trade and the trade object has been
	 * deleted from the server.
	 *
	 * @param tradeId  Unique trade identifier
	 * @param playerId Unique player identifier of rejecting player
	 */
	void msgTradeUpdateRejected(int tradeId, int playerId);

private:
	void writeData(QString msg);
	void processMsg(QString);
	void processNode(QDomNode);

	AtlanticCore *m_atlanticCore;
	Atlantik *m_mainWindow;
	QDomDocument msg;

	QMap<int, Player *> m_players;
	QMap<int, Estate *> m_estates;
	QMap<int, Trade *> m_trades;
};

#endif
