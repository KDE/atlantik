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

#include "network_defs.h"

#include <QMap>
#include <QTextStream>
#include <QDomNode>
#include <QAbstractSocket>

#include <libatlantikclient_export.h>

class AtlanticCore;
class Auction;
class Card;
class Estate;
class EstateGroup;
class Player;
class Trade;
class QTcpSocket;

class  LIBATLANTIKCLIENT_EXPORT AtlantikNetwork: public QObject
{
Q_OBJECT

public:
	AtlantikNetwork(AtlanticCore *atlanticCore);
	virtual ~AtlantikNetwork(void);

	void setName(const QString &name);
	void cmdChat(const QString &msg);

	QString host() const;
	int port() const;

	bool isConnected() const;

	void reset();
	void disconnect();

private Q_SLOTS:
	void slotwriteData(const QString &msg);
	void slotLookupFinished();
	void slotConnectionSuccess();
	void slotConnectionFailed(QAbstractSocket::SocketError error);
	void slotDisconnected();

	void slotRead();
	void writeData(const QString &data);


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
	void estateSell(Estate *estate);
	void jailCard();
	void jailPay();
	void jailRoll();
	void newTrade(Player *player);
	void kickPlayer(Player *player);
	void tokenConfirmation(Estate *);
	void tradeUpdateEstate(Trade *trade, Estate *estate, Player *player);
	void tradeUpdateMoney(Trade *trade, unsigned int money, Player *pFrom, Player *pTo);
	void tradeUpdateCard(Trade *trade, Card *card, Player *pTo);
	void tradeReject(Trade *trade);
	void tradeAccept(Trade *trade);
	void auctionBid(Auction *auction, int amount);
	void changeOption(int, const QString &value);



public Q_SLOTS:
	void serverConnect(const QString &host, int port);

	void joinGame(int gameId);
	void watchGame(int gameId);
	void leaveGame();
	void setImage(const QString &name);

Q_SIGNALS:
	void connectionSuccess();
	void connectionFailed(QAbstractSocket::SocketError error);
	void disconnected();

	void msgInfo(const QString &);
	void msgError(const QString &);
	void msgChat(const QString &, const QString &);
	void msgStatus(const QString &data, EventType type);
	void networkEvent(const QString &data, EventType type);
	void displayDetails(const QString &text, bool clearText, bool clearButtons, Estate *estate = nullptr);
	void displayText(const QString &text, bool clearText, bool clearButtons);
	void addCommandButton(const QString &command, const QString &caption, bool enabled);
	void addCloseButton();

	void gameOption(const QString &title, const QString &type, const QString &value, const QString &edit, const QString &command);
	void endConfigUpdate();

	void gameConfig();
	void gameInit();
	void gameRun();
	void gameEnd();
	void receivedHandshake();
	void clientCookie(const QString &cookie);
	void newEstate(Estate *estate);

	void newEstateGroup(EstateGroup *estateGroup);
	void msgTradeUpdateAccepted(Trade *trade);

	void msgTradeUpdateRejected(Trade *trade, int playerId);

	void newAuction(Auction *auction);
	void auctionCompleted(Auction *auction);
private:
        void processMsg(const QString &msg);
        void processNode(QDomNode);
	AtlanticCore *m_atlanticCore;
	QTcpSocket *m_monopdsocket;
	QTextStream m_monopdstream;
	int m_playerId;
	QString m_serverVersion;
	QMap<Player *, int> m_playerLocationMap;
	QString m_host;
	int m_port;
};

#endif
