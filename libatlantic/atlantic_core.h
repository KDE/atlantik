// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef LIBATLANTIC_CORE_H
#define LIBATLANTIC_CORE_H

#include <qobject.h>
#include <qptrlist.h>

class Player;
class Estate;
class EstateGroup;
class Trade;
class Auction;

class AtlanticCore : public QObject
{
Q_OBJECT

public:
	AtlanticCore(QObject *parent, const char *name);

	QPtrList<Player> players();
	Player *newPlayer();

	QPtrList<Estate> estates();
	Estate *newEstate(int estateId);

	QPtrList<EstateGroup> estateGroups();
	EstateGroup *newEstateGroup(QString name);

	QPtrList<Trade> trades();
	Trade *newTrade(int tradeId);
	void delTrade(Trade *trade);

	QPtrList<Auction> auctions();
	Auction *newAuction(int auctionId, Estate *estate);
	void delAuction(Auction *auction);

	void setCurrentTurn(Player *player);

signals:
	void removeGUI(Trade *trade);

private:
	QPtrList<Player> m_players;
	QPtrList<Estate> m_estates;
	QPtrList<EstateGroup> m_estateGroups;
	QPtrList<Trade> m_trades;
	QPtrList<Auction> m_auctions;
};

#endif
