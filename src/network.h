#ifndef ATLANTIK_NETWORK_H
#define ATLANTIK_NETWORK_H

#include <qsocket.h>
#include <qdom.h>
#include <qptrlist.h>
#include <qmap.h>

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
	void cmdTokenConfirmation(int estateId);
	void cmdEstateMortgage(int estateId);
	void cmdEstateUnmortgage(int estateId);
	void cmdHouseBuy(int estateId);
	void cmdHouseSell(int estateId);
	void cmdGamesList();
	void cmdGameNew();
	void cmdGameJoin(int gameId);
	void cmdTradeNew();
	void cmdTradeToggleEstate(int tradeId, int estateId);
	void cmdTradeSetMoney(int tradeId, int amount);
	void cmdTradeAccept(int tradeId);
	void cmdTradeReject(int tradeId);
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
	void msgEstateUpdateMortgaged(int, bool);
	void msgEstateUpdateCanBeMortgaged(int, bool);
	void msgEstateUpdateCanBeUnmortgaged(int, bool);
	void msgEstateUpdateCanBeOwned(int, bool);

	/**
	 * A new trade is created on the server.
	 *
	 * @param tradeId  Unique identifier of the trade
	 * @param playerId Player who initiates the trade
	 */
	void msgTradeUpdateNew(int tradeId, int playerId);

	/**
	 * A player should be added to the trade.
	 *
	 * @param tradeId  Unique identifier of the trade
	 * @param playerId Player participating in the trade
	 */
	void msgTradeUpdatePlayerAdd(int tradeId, int playerId);

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
	 * @param included Whether the estate is included
	 */
	void msgTradeUpdateEstateIncluded(int tradeId, int estateId, bool included);

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

	/**
	 * A new player object must be initialized.
	 *
	 * @param playerid Playerid for the new object
	 */
	void playerInit(int playerid);

	/**
	 * A new estate object must be initialized.
	 *
	 * @param estateid Estateid for the new object
	 */
	void estateInit(int estateid);

	void setPlayerId(int);
	void setTurn(int);

private:
	void writeData(QString msg);
	void processMsg(QString);
	void processNode(QDomNode);

	QDomDocument msg;
//	QPtrList<PortfolioView> portfolioViewList;
};

extern GameNetwork *gameNetwork;

#endif
