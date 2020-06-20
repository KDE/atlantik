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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_AUCTION_WIDGET_H
#define ATLANTIK_AUCTION_WIDGET_H

#include "estatedetailsbase.h"

#include <QMap>

class QTreeWidgetItem;

class AtlanticCore;
class Player;
class Auction;

namespace Ui
{
class AuctionWidget;
}

class AuctionWidget : public EstateDetailsBase
{
Q_OBJECT

public:
	AuctionWidget(AtlanticCore *atlanticCore, Auction *auction, QWidget *parent);
	~AuctionWidget();

private Q_SLOTS:
	void auctionChanged();
	void playerChanged(Player *player);
	void playerCreated(Player *player);
	void playerRemoved(Player *player);
	void updateBid(Player *player, int amount);
	void slotBidButtonClicked();

Q_SIGNALS:
	void bid(Auction *auction, int amount);

private:
	QTreeWidgetItem *createPlayerItem(Player *player);

	Ui::AuctionWidget *m_ui;
	QMap<Player *, QTreeWidgetItem *> m_playerItems;

	AtlanticCore *m_atlanticCore;
	Auction *m_auction;
};

#endif
