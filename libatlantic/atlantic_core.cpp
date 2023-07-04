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

#include <algorithm>
#include <iostream>

#include "atlantic_core.h"

#include "auction.h"
#include "card.h"
#include "configoption.h"
#include "estate.h"
#include "estategroup.h"
#include "game.h"
#include "player.h"
#include "trade.h"

AtlanticCore::AtlanticCore(QObject *parent)
	: QObject(parent)
	, m_playerSelf(nullptr)
{
}

void AtlanticCore::reset(bool deletePermanents)
{
	qDeleteAll(m_auctions);
	m_auctions.clear();
	qDeleteAll(m_estates);
	m_estates.clear();
	qDeleteAll(m_estateGroups);
	m_estateGroups.clear();
	qDeleteAll(m_cards);
	m_cards.clear();

	foreach (Trade *trade, m_trades)
	{
		Q_EMIT removeGUI(trade);
		trade->deleteLater();
	}
	m_trades.clear();

	foreach (Player *player, m_players)
	{
		if (deletePermanents)
		{
			Q_EMIT removeGUI(player);
			player->deleteLater();
		}
		else
		{
			player->setLocation(nullptr);
			player->setDestination(nullptr);
		}
	}
	if (deletePermanents)
	{
		m_players.clear();
		m_playerSelf = nullptr;

		foreach (Game *game, m_games)
		{
			Q_EMIT removeGUI(game);
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
	if (player == m_playerSelf)
		return;

	Player *oldSelf = m_playerSelf;
	m_playerSelf = player;
	if (oldSelf) {
		oldSelf->setIsSelf(false);
		oldSelf->update(true);
	}
	if (player) {
		player->setIsSelf(true);
		player->update(true);
	}
}

Player *AtlanticCore::playerSelf() const
{
	return m_playerSelf;
}

QList<Player *> AtlanticCore::players() const
{
	return m_players;
}

Player *AtlanticCore::newPlayer(int playerId, bool playerSelf)
{
	Player *player = new Player(playerId);
	m_players.append(player);

	if (playerSelf)
	{
		player->setIsSelf(playerSelf);
		m_playerSelf = player;
	}

	Q_EMIT createGUI(player);

	return player;
}

Player *AtlanticCore::findPlayer(int playerId) const
{
	foreach (Player *player, m_players)
		if (player->id() == playerId)
			return player;

	return nullptr;
}

void AtlanticCore::removePlayer(Player *player)
{
	m_players.removeOne(player);
	Q_EMIT removeGUI(player);
	if (player == m_playerSelf)
		m_playerSelf = nullptr;
	player->deleteLater();
}

QList<Game *> AtlanticCore::games() const
{
	return m_games;
}

Game *AtlanticCore::newGame(int gameId, const QString &type)
{
	Game *game = new Game(gameId);
	m_games.append(game);

	if ( !type.isNull() )
		game->setType(type);

	Q_EMIT createGUI(game);

	return game;
}

Game *AtlanticCore::findGame(const QString &type) const
{
	foreach (Game *game, m_games)
		if (game->id() == -1 && game->type() == type)
			return game;

	return nullptr;
}

Game *AtlanticCore::findGame(int gameId) const
{
	if (gameId == -1)
		return nullptr;

	foreach (Game *game, m_games)
		if (game->id() == gameId)
			return game;

	return nullptr;
}

Game *AtlanticCore::gameSelf() const
{
	return( m_playerSelf ? m_playerSelf->game() : nullptr );
}

void AtlanticCore::removeGame(Game *game)
{
	m_games.removeOne(game);
	foreach (Player *player, m_players)
		if (player->game() && player->game()->id() == game->id())
		{
			player->setGame(nullptr);
			player->update();
		}
	Q_EMIT removeGUI(game);
	game->deleteLater();
}

void AtlanticCore::emitGames()
{
	foreach (Game *game, m_games)
		Q_EMIT createGUI(game);
}

QList<Estate *> AtlanticCore::estates() const
{
	return m_estates;
}

Estate *AtlanticCore::newEstate(int estateId)
{
	Estate *estate = new Estate(estateId);
	m_estates.append(estate);
	return estate;
}

Estate *AtlanticCore::findEstate(int estateId) const
{
	foreach (Estate *estate, m_estates)
		if (estate->id() == estateId)
			return estate;

	return nullptr;
}

Estate *AtlanticCore::estateAfter(Estate *estate) const
{
	if (m_estates.isEmpty())
		return nullptr;
	const QList<Estate *>::const_iterator itEnd = m_estates.constEnd();
	QList<Estate *>::const_iterator it = std::find(m_estates.constBegin(), itEnd, estate);
	if (it == itEnd)
		return m_estates.at(0);
	++it;
	return it != itEnd ? *it : m_estates.at(0);
}

QList<EstateGroup *> AtlanticCore::estateGroups() const
{
	return m_estateGroups;
}

EstateGroup *AtlanticCore::newEstateGroup(int groupId)
{
	EstateGroup *estateGroup = new EstateGroup(groupId);
	m_estateGroups.append(estateGroup);
	return estateGroup;
}

EstateGroup *AtlanticCore::findEstateGroup(int groupId) const
{
	foreach (EstateGroup *estateGroup, m_estateGroups)
		if (estateGroup->id() == groupId)
			return estateGroup;

	return nullptr;
}

QList<Trade *> AtlanticCore::trades() const
{
	return m_trades;
}

Trade *AtlanticCore::newTrade(int tradeId)
{
	Trade *trade = new Trade(tradeId);
	m_trades.append(trade);

	Q_EMIT createGUI(trade);

	return trade;
}

Trade *AtlanticCore::findTrade(int tradeId) const
{
	foreach (Trade *trade, m_trades)
		if (trade->tradeId() == tradeId)
			return trade;

	return nullptr;
}

void AtlanticCore::removeTrade(Trade *trade)
{
	m_trades.removeOne(trade);
	Q_EMIT removeGUI(trade);
	trade->deleteLater();
}

QList<Auction *> AtlanticCore::auctions() const
{
	return m_auctions;
}

Auction *AtlanticCore::newAuction(int auctionId, Estate *estate)
{
	Auction *auction = new Auction(auctionId, estate);
	m_auctions.append(auction);
	return auction;
}

Auction *AtlanticCore::findAuction(int auctionId) const
{
	foreach (Auction *auction, m_auctions)
		if (auction->auctionId() == auctionId)
			return auction;

	return nullptr;
}

void AtlanticCore::delAuction(Auction *auction)
{
	m_auctions.removeOne(auction);
	delete auction;
}

QList<Card *> AtlanticCore::cards() const
{
	return m_cards;
}

Card *AtlanticCore::newCard(int cardId)
{
	Card *card = new Card(cardId);
	m_cards.append(card);
	return card;
}

Card *AtlanticCore::findCard(int cardId) const
{
	foreach (Card *card, m_cards)
		if (card->cardId() == cardId)
			return card;

	return nullptr;
}

void AtlanticCore::printDebug() const
{
#define LP(string) (string).toLatin1().constData()

	foreach (Player *player, m_players)
		if (player == m_playerSelf)
			std::cout << "PS: " << LP(player->name()) << ", game " << (player->game() ? player->game()->id() : -1) << std::endl;
		else
			std::cout << " P: " << LP(player->name()) << ", game " << (player->game() ? player->game()->id() : -1) << std::endl;

	foreach (Game *game, m_games)
	{
		std::cout << " G: " << game->id() << ", master: " << (game->master() ? game->master()->id() : -1 ) << std::endl;
		foreach (ConfigOption *configOption, game->configOptions())
			std::cout << "    CO:" << configOption->id() << " " << LP(configOption->name()) << " " << LP(configOption->value()) << std::endl;
	}

	foreach (Estate *estate, m_estates)
		std::cout << " E: " << LP(estate->name()) << std::endl;

	foreach (EstateGroup *estateGroup, m_estateGroups)
		std::cout << "EG: " << LP(estateGroup->name()) << std::endl;

	foreach (Auction *auction, m_auctions)
		std::cout << " A: " << auction->auctionId() << std::endl;

	foreach (Trade *trade, m_trades)
		std::cout << " T: " << trade->tradeId() << std::endl;

	foreach (Card *card, m_cards)
		std::cout << "CA: " << card->cardId() << std::endl;

#undef LP
}

#include "moc_atlantic_core.cpp"
