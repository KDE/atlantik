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

#include "atlantic_core.moc"

#include "player.h"
#include "estate.h"
#include "estategroup.h"
#include "trade.h"
#include "auction.h"

AtlanticCore::AtlanticCore(QObject *parent, const char *name) : QObject(parent, name)
{
	m_playerSelf = 0;
}

void AtlanticCore::reset(bool deletePlayers)
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
		if (deletePlayers)
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
	if (deletePlayers)
	{
		m_players.clear();
		m_playerSelf = 0;
	}
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

Player *AtlanticCore::newPlayer(int playerId)
{
	Player *player = new Player(playerId);
	m_players.append(player);
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
		if (estate->estateId() == estateId)
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
