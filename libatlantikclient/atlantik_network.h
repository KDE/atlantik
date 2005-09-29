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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef LIBATLANTIK_NETWORK_H
#define LIBATLANTIK_NETWORK_H

#include <qmap.h>

#include <kextsock.h>
#include "libatlantic_export.h"
class QDomNode;
class QTextStream;

class AtlanticCore;

class Player;
class Estate;
class EstateGroup;
class Trade;
class Auction;

class LIBATLANTIC_EXPORT AtlantikNetwork : public KExtendedSocket
{
Q_OBJECT

public:
	AtlantikNetwork(AtlanticCore *atlanticCore);
	virtual ~AtlantikNetwork(void);
	void setName(QString name);
	void cmdChat(QString msg);

private slots:
	void writeData(QString msg);
	void rollDice();
	void endTurn();
	void newGame(const QString &gameType);
	void reconnect(const QString &cookie);
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
	void kickPlayer(Player *player);
	void tokenConfirmation(Estate *);
	void tradeUpdateEstate(Trade *trade, Estate *estate, Player *player);
	void tradeUpdateMoney(Trade *trade, unsigned int money, Player *pFrom, Player *pTo);
	void tradeReject(Trade *trade);
	void tradeAccept(Trade *trade);
	void auctionBid(Auction *auction, int amount);
	void changeOption(int, const QString &value);
	void slotLookupFinished(int count);
	void slotConnectionSuccess();
	void slotConnectionFailed(int error);

public slots:
	void serverConnect(const QString host, int port);
	void joinGame(int gameId);
	void leaveGame();
	void slotRead();
	void setImage(const QString &name);

signals:
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

	void msgInfo(QString);
	void msgError(QString);
	void msgChat(QString, QString);
	void msgStatus(const QString &data, const QString &icon = QString::null);
	void networkEvent(const QString &data, const QString &icon);

	void displayDetails(QString text, bool clearText, bool clearButtons, Estate *estate = 0);
	void addCommandButton(QString command, QString caption, bool enabled);
	void addCloseButton();

	void gameOption(QString title, QString type, QString value, QString edit, QString command);
	void endConfigUpdate();

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
	void receivedHandshake();
	void clientCookie(QString cookie);

private:
	void processMsg(const QString &msg);
	void processNode(QDomNode);

	AtlanticCore *m_atlanticCore;
	QTextStream *m_textStream;

	int m_playerId;
	QString m_serverVersion;

	QMap<Player *, int> m_playerLocationMap;
	QMap<int, Auction *> m_auctions;
};

#endif
