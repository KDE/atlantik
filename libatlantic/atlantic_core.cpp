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
	connect(this, SIGNAL(deletePlayer(Player *)), this, SLOT(slotDeletePlayer(Player *)));
	connect(this, SIGNAL(deleteTrade(Trade *)), this, SLOT(slotDeleteTrade(Trade *)));
}

void AtlanticCore::reset()
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
	m_players.setAutoDelete(true);
	m_players.clear();
	m_players.setAutoDelete(false);
	m_trades.setAutoDelete(true);
	m_trades.clear();
	m_trades.setAutoDelete(false);
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
	emit deletePlayer(player);
}

void AtlanticCore::slotDeletePlayer(Player *player)
{
	delete player;
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

void AtlanticCore::removeTrade(Trade *trade)
{
	m_trades.remove(trade);
	emit removeGUI(trade);
	emit deleteTrade(trade);
}

void AtlanticCore::slotDeleteTrade(Trade *trade)
{	
	delete trade;
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
