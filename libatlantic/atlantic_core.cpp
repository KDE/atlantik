// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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
	connect(this, SIGNAL(deleteTrade(Trade *)), this, SLOT(slotDeleteTrade(Trade *)));
}

QPtrList<Player> AtlanticCore::players()
{
	return m_players;
}

Player *AtlanticCore::newPlayer()
{
	Player *player = new Player();
	m_players.append(player);
	return player;
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

QPtrList<EstateGroup> AtlanticCore::estateGroups()
{
	return m_estateGroups;
}

EstateGroup *AtlanticCore::newEstateGroup(const int id)
{
	EstateGroup *estateGroup = new EstateGroup(id);
	m_estateGroups.append(estateGroup);
	return estateGroup;
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
