#ifndef LIBATLANTIK_NETWORK_H
#define LIBATLANTIK_NETWORK_H

#include <qdom.h>
#include <qmap.h>

#ifndef USE_KDE
#include <qsocket.h>
#else
#include <kextsock.h>
#endif

class AtlanticCore;

class Player;
class Estate;
class Trade;
class Auction;

//#ifndef USE_KDE
class AtlantikNetwork : public QSocket
//#else
//class AtlantikNetwork : public KExtendedSocket
//#endif
{
Q_OBJECT

public:
	AtlantikNetwork(AtlanticCore *atlanticCore, QObject *parent=0, const char *name=0);
	void cmdName(QString name);
	void cmdGamesList();
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
	void auctionEstate();
	void estateToggleMortgage(Estate *estate);
	void estateHouseBuy(Estate *estate);
	void estateHouseSell(Estate *estate);
	void jailCard();
	void jailPay();
	void jailRoll();
	void newTrade(Player *player);
	void tokenConfirmation(Estate *);
	void tradeUpdateEstate(Trade *trade, Estate *estate, Player *player);
	void tradeUpdateMoney(Trade *trade, unsigned int money, Player *pFrom, Player *pTo);
	void auctionBid(Auction *auction, int amount);

public slots:
	void serverConnect(const QString host, int port);
	void slotRead();
		
signals:

	/**
	 * A new player was created. This signal might be replaced with one in
	 * the AtlanticCore class in the future, but it is here now because we
	 * do not want GUI implementations to create a view until the
	 * playerupdate message has been fully parsed.
	 *
	 * @param player	Point to created player object.
	 */
	void newPlayer(Player *player);

	/**
	 * A new estate was created. This signal might be replaced with one in
	 * the AtlanticCore class in the future, but it is here now because we
	 * do not want GUI implementations to create a view until the
	 * estateupdate message has been fully parsed.
	 *
	 * @param estate	Point to created estate object.
	 */
	void newEstate(Estate *estate);

	/**
	 * A new trade was created. This signal might be replaced with one in
	 * the AtlanticCore class in the future, but it is here now because we
	 * do not want GUI implementations to create a view until the
	 * tradeupdate message has been fully parsed.
	 *
	 * @param trade	Point to created trade object.
	 */
	void newTrade(Trade *trade);

	void msgInfo(QString);
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

	void newAuction(Auction *auction);
	void auctionCompleted(Auction *auction);

private:
	void writeData(QString msg);
	void processMsg(QString);
	void processNode(QDomNode);

	AtlanticCore *m_atlanticCore;
	QObject *m_parent;
	QDomDocument msg;

	int m_clientId, m_playerId;

	QMap<int, Player *> m_players;
	QMap<int, Estate *> m_estates;
	QMap<int, Trade *> m_trades;
	QMap<int, Auction *> m_auctions;
};

#endif
