// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef LIBATLANTIK_NETWORK_H
#define LIBATLANTIK_NETWORK_H

#include <qdom.h>
#include <qmap.h>

#include <kextsock.h>

class AtlanticCore;

class Player;
class Estate;
class EstateGroup;
class Trade;
class Auction;

class QTextStream;

class AtlantikNetwork : public KExtendedSocket
{
Q_OBJECT

public:
	AtlantikNetwork(AtlanticCore *atlanticCore);
	virtual ~AtlantikNetwork(void);
	void setName(QString name);
	void cmdGamesList();
	void cmdChat(QString msg);

private slots:
	void writeData(QString msg);
	void rollDice();
	void endTurn();
	void getPlayerList();
	void newGame(const QString &gameType);
	void startGame();
	void leaveGame();
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
	void tradeReject(Trade *trade);
	void tradeAccept(Trade *trade);
	void auctionBid(Auction *auction, int amount);

public slots:
	void serverConnect(const QString host, int port);
	void joinGame(int gameId);
	void slotRead();
		
signals:

	/**
	 * A new player was created. This signal might be replaced with one in
	 * the AtlanticCore class in the future, but it is here now because we
	 * do not want GUI implementations to create a view until the
	 * playerupdate message has been fully parsed.
	 *
	 * @param player	Created Player object.
	 */
	void newPlayer(Player *player);

	/**
	 * A new estate was created. This signal might be replaced with one in
	 * the AtlanticCore class in the future, but it is here now because we
	 * do not want GUI implementations to create a view until the
	 * estateupdate message has been fully parsed.
	 *
	 * @param estate	Created Estate object.
	 */
	void newEstate(Estate *estate);

	/**
	 * A new estate group was created. This signal might be replaced with
	 * one in the AtlanticCore class in the future, but it is here now
	 * because we do not want GUI implementations to create a view until the
	 * estategroupupdate message has been fully parsed.
	 *
	 * @param estateGroup	Created EstateGroup object.
	 */
	void newEstateGroup(EstateGroup *estateGroup);

	/**
	 * A new trade was created. This signal might be replaced with one in
	 * the AtlanticCore class in the future, but it is here now because we
	 * do not want GUI implementations to create a view until the
	 * tradeupdate message has been fully parsed.
	 *
	 * @param trade	Created Trade object.
	 */
	void newTrade(Trade *trade);

	void msgInfo(QString);
	void msgError(QString);
	void msgChat(QString, QString);

	void displayText(QString title, QString description);
	void displayEstate(Estate *);
	void displayDefault();
	void addCommandButton(QString command, QString caption, bool enabled);
	void addCloseButton();

	void gameOption(QString title, QString type, QString value, QString edit, QString command);
	void gameListClear();
	void gameListEndUpdate();
	void gameListAdd(QString gameId, QString name, QString description, QString players, QString gameType, bool canBeJoined);
	void gameListEdit(QString gameId, QString name, QString description, QString players, QString gameType, bool canBeJoined);
	void gameListDel(QString gameId);

	void gameConfig();
	void gameInit();
	void gameRun();
	void gameEnd();

	/**
	 * The trade has been completed. Emitted after all necessary estate and
	 * player updates are processed.
	 *
	 * @param trade Trade
	 */
	void msgTradeUpdateAccepted(Trade *trade);

	/**
	 * One of the players rejected the trade and the trade object has been
	 * deleted from the server.
	 *
	 * @param trade Trade
	 * @param playerId Unique player identifier of rejecting player
	 */
	void msgTradeUpdateRejected(Trade *trade, int playerId);

	void newAuction(Auction *auction);
	void auctionCompleted(Auction *auction);

private:
	void processMsg(QString);
	void processNode(QDomNode);

	AtlanticCore *m_atlanticCore;
	QTextStream *m_textStream;
	QDomDocument msg;

	int m_playerId;

	QMap<Player *, int> m_playerLocationMap;
	QMap<int, Trade *> m_trades;
	QMap<int, Auction *> m_auctions;
};

#endif
