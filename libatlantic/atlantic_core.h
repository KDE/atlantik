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

#ifndef LIBATLANTIC_CORE_H
#define LIBATLANTIC_CORE_H

#include <QList>
#include <QObject>

#include "libatlantic_export.h"

class Player;
class Estate;
class EstateGroup;
class Game;
class Trade;
class Auction;
class Card;

class LIBATLANTIC_EXPORT AtlanticCore : public QObject
{
Q_OBJECT

public:
	AtlanticCore(QObject *parent);

	void reset(bool deletePermanents = false);

	bool selfIsMaster() const;

	void setPlayerSelf(Player *player);
	Player *playerSelf() const;

	QList<Player *> players() const;
	Player *newPlayer(int playerId, bool playerSelf = false);
	Player *findPlayer(int playerId) const;
	void removePlayer(Player *player);

	QList<Game *> games() const;
	Game *newGame(int gameId, const QString &type = QString());
	Game *findGame(const QString &type) const; // finds game types
	Game *findGame(int gameId) const; // finds actual games
	Game *gameSelf() const;
	void removeGame(Game *game);
	void emitGames();

	QList<Estate *> estates() const;
	Estate *newEstate(int estateId);
	Estate *findEstate(int estateId) const;
	Estate *estateAfter(Estate *estate) const;

	QList<EstateGroup *> estateGroups() const;
	EstateGroup *newEstateGroup(int groupId);
	EstateGroup *findEstateGroup(int groupId) const;

	QList<Trade *> trades() const;
	Trade *newTrade(int tradeId);
	Trade *findTrade(int tradeId) const;
	void removeTrade(Trade *trade);

	QList<Auction *> auctions() const;
	Auction *newAuction(int auctionId, Estate *estate);
	Auction *findAuction(int auctionId) const;
	void delAuction(Auction *auction);

	QList<Card *> cards() const;
	Card *newCard(int cardId);
	Card *findCard(int cardId) const;

	void printDebug() const;

Q_SIGNALS:
	void createGUI(Player *player);
	void removeGUI(Player *player);
	void createGUI(Game *game);
	void removeGUI(Game *game);
	void createGUI(Trade *trade);
	void removeGUI(Trade *trade);

private:
	Player *m_playerSelf;
	QList<Player *> m_players;
	QList<Game *> m_games;
	QList<Estate *> m_estates;
	QList<EstateGroup *> m_estateGroups;
	QList<Trade *> m_trades;
	QList<Auction *> m_auctions;
	QList<Card *> m_cards;
};

#endif
