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

	void reset();

	QPtrList<Player> players();
	Player *newPlayer(int playerId);
	Player *findPlayer(int playerId);
	void removePlayer(Player *player);

	QPtrList<Estate> estates();
	Estate *newEstate(int estateId);
	Estate *findEstate(int estateId);
	Estate *estateAfter(Estate *estate);

	QPtrList<EstateGroup> estateGroups();
	EstateGroup *newEstateGroup(int groupId);
	EstateGroup *findEstateGroup(int groupId);

	QPtrList<Trade> trades();
	Trade *newTrade(int tradeId);
	void removeTrade(Trade *trade);

	QPtrList<Auction> auctions();
	Auction *newAuction(int auctionId, Estate *estate);
	void delAuction(Auction *auction);

	void printDebug();

signals:
	void removeGUI(Player *player);
	void deletePlayer(Player *player);
	void removeGUI(Trade *trade);
	void deleteTrade(Trade *trade);

private slots:
	void slotDeletePlayer(Player *player);
	void slotDeleteTrade(Trade *trade);

private:
	QPtrList<Player> m_players;
	QPtrList<Estate> m_estates;
	QPtrList<EstateGroup> m_estateGroups;
	QPtrList<Trade> m_trades;
	QPtrList<Auction> m_auctions;
};

#endif
