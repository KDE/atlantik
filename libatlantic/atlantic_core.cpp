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

#include <iostream>

#include "atlantic_core.h"

#include "auction.h"
#include "configoption.h"
#include "estate.h"
#include "estategroup.h"
#include "game.h"
#include "player.h"
#include "trade.h"

AtlanticCore::AtlanticCore(QObject *parent, const char *name) : QObject(parent, name)
{
	m_playerSelf = 0;
}

void AtlanticCore::reset(bool deletePermanents)
{
	m_auctions.setAutoDelete(true);
	m_auctions.clear();
	m_auctions.setAutoDelete(false);
	m_estates.setAutoDelete(true);
	m_estates.clear();
	m_estates.setAutoDelete(false);
	m_estateGroups.setAutoDelete(true);
	m_estateGroups.clear();
	m_estateGroups.setAutoDelete(false);
	m_configOptions.setAutoDelete(true);
	m_configOptions.clear();
	m_configOptions.setAutoDelete(false);

	Trade *trade = 0;
	for (QPtrListIterator<Trade> it(m_trades); (trade = *it) ; ++it)
	{
		emit removeGUI(trade);
		trade->deleteLater();
	}
	m_trades.clear();

	Player *player = 0;
	for (QPtrListIterator<Player> it(m_players); (player = *it) ; ++it)
	{
		if (deletePermanents)
		{
			emit removeGUI(player);
			player->deleteLater();
		}
		else
		{
			player->setLocation(0);
			player->setDestination(0);
		}
	}
	if (deletePermanents)
	{
		m_players.clear();
		m_playerSelf = 0;

		Game *game = 0;
		for (QPtrListIterator<Game> it(m_games); (game = *it) ; ++it)
		{
			emit removeGUI(game);
			game->deleteLater();
		}
		m_games.clear();
	}
}

bool AtlanticCore::selfIsMaster() const
{
	return (m_playerSelf && m_playerSelf->game() && m_playerSelf->game()->master() == m_playerSelf);
}

void AtlanticCore::setPlayerSelf(Player *player)
{
	m_playerSelf = player;
}

Player *AtlanticCore::playerSelf()
{
	return m_playerSelf;
}

QPtrList<Player> AtlanticCore::players()
{
	return m_players;
}

Player *AtlanticCore::newPlayer(int playerId, const bool &playerSelf)
{
	Player *player = new Player(playerId);
	m_players.append(player);

	if (playerSelf)
	{
		player->setIsSelf(playerSelf);
		m_playerSelf = player;
	}

	emit createGUI(player);

	return player;
}

Player *AtlanticCore::findPlayer(int playerId)
{
	Player *player = 0;
	for (QPtrListIterator<Player> it(m_players); (player = *it) ; ++it)
		if (player->id() == playerId)
			return player;

	return 0;
}

void AtlanticCore::removePlayer(Player *player)
{
	m_players.remove(player);
	emit removeGUI(player);
	player->deleteLater();
}

QPtrList<Game> AtlanticCore::games()
{
	return m_games;
}

Game *AtlanticCore::newGame(int gameId, const QString &type)
{
	Game *game = new Game(gameId);
	m_games.append(game);

	if ( !type.isNull() )
		game->setType(type);

	emit createGUI(game);

	return game;
}

Game *AtlanticCore::findGame(const QString &type)
{
	Game *game = 0;
	for (QPtrListIterator<Game> it(m_games); (game = *it) ; ++it)
		if (game->id() == -1 && game->type() == type)
			return game;

	return 0;
}

Game *AtlanticCore::findGame(int gameId)
{
	if (gameId == -1)
		return 0;

	Game *game = 0;
	for (QPtrListIterator<Game> it(m_games); (game = *it) ; ++it)
		if (game->id() == gameId)
			return game;

	return 0;
}

Game *AtlanticCore::gameSelf()
{
	return( m_playerSelf ? m_playerSelf->game() : 0 );
}

void AtlanticCore::removeGame(Game *game)
{
	m_games.remove(game);
	emit removeGUI(game);
	game->deleteLater();
}

void AtlanticCore::emitGames()
{
	for (QPtrListIterator<Game> it(m_games); (*it) ; ++it)
		emit createGUI( (*it) );
}

QPtrList<Estate> AtlanticCore::estates()
{
	return m_estates;
}

Estate *AtlanticCore::newEstate(int estateId)
{
	Estate *estate = new Estate(estateId);
	m_estates.append(estate);
	return estate;
}

Estate *AtlanticCore::findEstate(int estateId)
{
	Estate *estate = 0;
	for (QPtrListIterator<Estate> it(m_estates); (estate = *it) ; ++it)
		if (estate->id() == estateId)
			return estate;

	return 0;
}

Estate *AtlanticCore::estateAfter(Estate *estate)
{
	Estate *eFirst = 0, *eTmp = 0;
	bool useNext = false;
	for (QPtrListIterator<Estate> it(m_estates); (eTmp = *it) ; ++it)
	{
		if (!eFirst)
			eFirst = eTmp;
		if (eTmp == estate)
			useNext = true;
		else if (useNext)
			return eTmp;
	}
	return eFirst;
}

QPtrList<EstateGroup> AtlanticCore::estateGroups()
{
	return m_estateGroups;
}

EstateGroup *AtlanticCore::newEstateGroup(int groupId)
{
	EstateGroup *estateGroup = new EstateGroup(groupId);
	m_estateGroups.append(estateGroup);
	return estateGroup;
}

EstateGroup *AtlanticCore::findEstateGroup(int groupId)
{
	EstateGroup *estateGroup = 0;
	for (QPtrListIterator<EstateGroup> it(m_estateGroups); (estateGroup = *it) ; ++it)
		if (estateGroup->id() == groupId)
			return estateGroup;

	return 0;
}

QPtrList<Trade> AtlanticCore::trades()
{
	return m_trades;
}

Trade *AtlanticCore::newTrade(int tradeId)
{
	Trade *trade = new Trade(tradeId);
	m_trades.append(trade);

	emit createGUI(trade);

	return trade;
}

Trade *AtlanticCore::findTrade(int tradeId)
{
	Trade *trade = 0;
	for (QPtrListIterator<Trade> it(m_trades); (trade = *it) ; ++it)
		if (trade->tradeId() == tradeId)
			return trade;

	return 0;
}

void AtlanticCore::removeTrade(Trade *trade)
{
	m_trades.remove(trade);
	emit removeGUI(trade);
	trade->deleteLater();
}

QPtrList<Auction> AtlanticCore::auctions()
{
	return m_auctions;
}

Auction *AtlanticCore::newAuction(int auctionId, Estate *estate)
{
	Auction *auction = new Auction(auctionId, estate);
	m_auctions.append(auction);
	return auction;
}

void AtlanticCore::delAuction(Auction *auction)
{
	m_auctions.remove(auction);
	delete auction;
}

ConfigOption *AtlanticCore::newConfigOption(int configId)
{
	ConfigOption *configOption = new ConfigOption(configId);
	m_configOptions.append(configOption);

	emit createGUI(configOption);

	return configOption;
}

void AtlanticCore::removeConfigOption(ConfigOption *configOption)
{
	m_configOptions.remove(configOption);
	emit removeGUI(configOption);
	configOption->deleteLater();
}

ConfigOption *AtlanticCore::findConfigOption(int configId)
{
	ConfigOption *configOption = 0;
	for (QPtrListIterator<ConfigOption> it(m_configOptions); (configOption = *it) ; ++it)
		if (configOption->id() == configId)
			return configOption;

	return 0;
}

void AtlanticCore::printDebug()
{
	Player *player = 0;
	for (QPtrListIterator<Player> it(m_players); (player = *it) ; ++it)
		if (player == m_playerSelf)
			std::cout << "PS: " << player->name().latin1() << ", game " << QString::number(player->game() ? player->game()->id() : -1).latin1() << std::endl;
		else
			std::cout << " P: " << player->name().latin1() << ", game " << QString::number(player->game() ? player->game()->id() : -1).latin1() << std::endl;

	Game *game = 0;
	for (QPtrListIterator<Game> it(m_games); (game = *it) ; ++it)
		std::cout << " G: " << QString::number(game->id()).latin1() << ", master: " << QString::number(game->master() ? game->master()->id() : -1 ).latin1() << std::endl;

	Estate *estate = 0;
	for (QPtrListIterator<Estate> it(m_estates); (estate = *it) ; ++it)
		std::cout << " E: " << estate->name().latin1() << std::endl;

	EstateGroup *estateGroup = 0;
	for (QPtrListIterator<EstateGroup> it(m_estateGroups); (estateGroup = *it) ; ++it)
		std::cout << "EG: " << estateGroup->name().latin1() << std::endl;

	Auction *auction = 0;
	for (QPtrListIterator<Auction> it(m_auctions); (auction = *it) ; ++it)
		std::cout << " A: " << QString::number(auction->auctionId()).latin1() << std::endl;

	Trade *trade = 0;
	for (QPtrListIterator<Trade> it(m_trades); (trade = *it) ; ++it)
		std::cout << " T: " << QString::number(trade->tradeId()).latin1() << std::endl;

	ConfigOption *configOption = 0;
	for (QPtrListIterator<ConfigOption> it(m_configOptions); (configOption = *it) ; ++it)
		std::cout << "CO:" << QString::number(configOption->id()).latin1() << " " << configOption->name().latin1() << " " << configOption->value().latin1() << std::endl;
}

#include "atlantic_core.moc"
