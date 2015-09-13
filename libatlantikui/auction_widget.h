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

#include <QWidget>
#include <QMap>

class QGroupBox;
class QSpinBox;
class QLabel;
class QVBoxLayout;
class QTreeWidget;
class QTreeWidgetItem;

class AtlanticCore;
class Player;
class Auction;

class AuctionWidget : public QWidget
{
Q_OBJECT

public:
	AuctionWidget(AtlanticCore *atlanticCore, Auction *auction, QWidget *parent);

private slots:
	void auctionChanged();
	void playerChanged(Player *player);
	void updateBid(Player *player, int amount);
	void slotBidButtonClicked();

signals:
	void bid(Auction *auction, int amount);

private:
	QVBoxLayout *m_mainLayout;
	QGroupBox *m_playerGroupBox;
	QSpinBox *m_bidSpinBox;
	QMap<Player *, QTreeWidgetItem *> m_playerItems;
	QLabel *m_statusLabel;

	QTreeWidget *m_playerList;

	AtlanticCore *m_atlanticCore;
	Auction *m_auction;
};

#endif
