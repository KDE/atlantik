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

#include <qobject.h>
#include <qptrlist.h>

#include "libatlantic_export.h"

class Player;
class ConfigOption;
class Estate;
class EstateGroup;
class Game;
class Trade;
class Auction;

class LIBATLANTIC_EXPORT AtlanticCore : public QObject
{
Q_OBJECT

public:
	AtlanticCore(QObject *parent, const char *name);

	void reset(bool deletePermanents = false);

	bool selfIsMaster() const;

	void setPlayerSelf(Player *player);
	Player *playerSelf();

	QPtrList<Player> players();
	Player *newPlayer(int playerId, const bool &playerSelf = false);
	Player *findPlayer(int playerId);
	void removePlayer(Player *player);

	QPtrList<Game> games();
	Game *newGame(int gameId, const QString &type = QString::null);
	Game *findGame(const QString &type); // finds game types
	Game *findGame(int gameId); // finds actual games
	Game *gameSelf();
	void removeGame(Game *game);
	void emitGames();

	QPtrList<Estate> estates();
	Estate *newEstate(int estateId);
	Estate *findEstate(int estateId);
	Estate *estateAfter(Estate *estate);

	QPtrList<EstateGroup> estateGroups();
	EstateGroup *newEstateGroup(int groupId);
	EstateGroup *findEstateGroup(int groupId);

	QPtrList<Trade> trades();
	Trade *newTrade(int tradeId);
	Trade *findTrade(int tradeId);
	void removeTrade(Trade *trade);

	QPtrList<Auction> auctions();
	Auction *newAuction(int auctionId, Estate *estate);
	void delAuction(Auction *auction);

	ConfigOption *newConfigOption(int configId);
	void removeConfigOption(ConfigOption *configOption);
	ConfigOption *findConfigOption(int configId);

	void printDebug();

signals:
	void createGUI(Player *player);
	void removeGUI(Player *player);
	void createGUI(Game *game);
	void removeGUI(Game *game);
	void createGUI(Trade *trade);
	void removeGUI(Trade *trade);
	void createGUI(ConfigOption *configOption);
	void removeGUI(ConfigOption *configOption);

private:
	Player *m_playerSelf;
	QPtrList<Player> m_players;
	QPtrList<Game> m_games;
	QPtrList<Estate> m_estates;
	QPtrList<EstateGroup> m_estateGroups;
	QPtrList<Trade> m_trades;
	QPtrList<Auction> m_auctions;
	QPtrList<ConfigOption> m_configOptions;
};

#endif
