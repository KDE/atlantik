#ifndef ATLANTIK_NETWORK_H
#define ATLANTIK_NETWORK_H

#include <qsocket.h>
#include <qdom.h>
#include <qptrlist.h>
#include <qmap.h>

class Atlantik;

class Trade;
class Estate;
class Player;

class GameNetwork : public QSocket
{
Q_OBJECT

public:
	GameNetwork(Atlantik *parent=0, const char *name=0);
	void cmdName(QString name);
	void cmdGamesList();
	void cmdTradeSetMoney(int tradeId, int amount);
	void cmdTradeAccept(int tradeId);
	void cmdTradeReject(int tradeId);
	void cmdChat(QString msg);

private slots:
	void roll();
	void endTurn();
	void newGame(const QString &gameType);
	void joinGame(int gameId);
	void startGame();
	void buyEstate();
	void estateToggleMortgage(int estateId);
	void estateHouseBuy(int estateId);
	void estateHouseSell(int estateId);
	void jailCard();
	void jailPay();
	void jailRoll();
	void newTrade(int playerId);
	void tradeUpdateEstate(Trade *trade, Estate *estate, Player *player);

public slots:
	void serverConnect(const QString host, int port);
	void cmdTokenConfirmation(int estateId);
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

	void msgPlayerUpdateName(int, QString);
	void msgPlayerUpdateMoney(int, QString);
	void msgPlayerUpdateJailed(int, bool);
	void msgPlayerUpdateLocation(int, int, bool);
	void playerUpdateFinished(int);
	void msgEstateUpdateName(int, QString);
	void msgEstateUpdateColor(int, QString);
	void msgEstateUpdateBgColor(int, QString);
	void msgEstateUpdateOwner(int, int);
	void msgEstateUpdateHouses(int, int);
	void msgEstateUpdateGroupId(int, int);
	void msgEstateUpdateMortgaged(int, bool);
	void msgEstateUpdateCanToggleMortgage(int, bool);
	void msgEstateUpdateCanBeOwned(int, bool);
	void estateUpdateCanBuyHouses(int, bool);
	void estateUpdateCanSellHouses(int, bool);
	void estateUpdateFinished(int);

	/**
	 * A new trade is created on the server or a current one is being updated.
	 *
	 * @param tradeId  Unique identifier of the trade
	 */
	void tradeInit(int tradeId);

	void tradeUpdateActor(int tradeId, int playerId);

	/**
	 * A player should be added to the trade.
	 *
	 * @param tradeId  Unique identifier of the trade
	 * @param playerId Player participating in the trade
	 */
	void tradeUpdatePlayerAdd(int tradeId, int playerId);

	/**
	 * Information from server whether a player accepts the trade as is.
	 *
	 * @param tradeId  Unique trade identifier
	 * @param playerId Unique player identifier
	 * @param accept   Whether the player accepts or not
	 */
	void msgTradeUpdatePlayerAccept(int tradeId, int playerId, bool accept);

	/**
	 * Information from server about the amount of money a player brings
	 * into a trade.
	 *
	 * @param tradeId  Unique trade identifier
	 * @param playerId Unique player identifier
	 * @param money    Amount of money
	 */
	void msgTradeUpdatePlayerMoney(int tradeId, int playerId, int money);

	/**
	 * Information from server whether an estate is included in a trade or not.
	 *
	 * @param tradeId  Unique trade identifier
	 * @param estateId Unique estate identifier
	 * @param playerId Unique player identifier, -1 for no player (not included in trade)
	 */
	void tradeUpdateEstate(int tradeId, int estateId, int playerId);

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

	void tradeUpdateFinished(int tradeId);

	/**
	 * A new player object must be initialized.
	 *
	 * @param playerId Playerid for the new object
	 */
	void playerInit(int playerId);

	/**
	 * A new estate object must be initialized.
	 *
	 * @param estateId Estateid for the new object
	 */
	void estateInit(int estateId);

	void setPlayerId(int);
	void setTurn(int);

private:
	void writeData(QString msg);
	void processMsg(QString);
	void processNode(QDomNode);

	Atlantik *m_parentWindow;
	QDomDocument msg;
//	QPtrList<PortfolioView> portfolioViewList;
};

extern GameNetwork *gameNetwork;

#endif
