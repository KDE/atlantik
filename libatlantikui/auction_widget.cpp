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

#include <qvgroupbox.h>
#include <qhbox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <kdebug.h>

#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpushbutton.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <auction.h>

#include "auction_widget.moc"

AuctionWidget::AuctionWidget(AtlanticCore *atlanticCore, Auction *auction, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = atlanticCore;

	m_auction = auction;
	connect(m_auction, SIGNAL(changed()), this, SLOT(auctionChanged()));
	connect(m_auction, SIGNAL(updateBid(Player *, int)), this, SLOT(updateBid(Player *, int)));
	connect(this, SIGNAL(bid(Auction *, int)), m_auction, SIGNAL(bid(Auction *, int)));

	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint());
	Q_CHECK_PTR(m_mainLayout);

	// Player list
	Estate *estate = auction->estate();
	m_playerGroupBox = new QVGroupBox(estate ? i18n("Auction: %1").arg(estate->name()) : i18n("Auction"), this, "groupBox");
	m_mainLayout->addWidget(m_playerGroupBox);

	m_playerList = new KListView(m_playerGroupBox);
	m_playerList->addColumn(i18n("Player"));
	m_playerList->addColumn(i18n("Bid"));
	m_playerList->setSorting(1, false);

	KListViewItem *item;
	Player *player, *pSelf = m_atlanticCore->playerSelf();

	QPtrList<Player> playerList = m_atlanticCore->players();
	for (QPtrListIterator<Player> it(playerList); *it; ++it)
	{
		if ( (player = *it) && player->game() == pSelf->game() )
		{
			item = new KListViewItem(m_playerList, player->name(), QString("0"));
			item->setPixmap(0, QPixmap(SmallIcon("personal")));
			m_playerItems[player] = item;

			connect(player, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
		}
	}

	// Bid spinbox and button
	QHBox *bidBox = new QHBox(this);
	m_mainLayout->addWidget(bidBox);

	m_bidSpinBox = new QSpinBox(1, 10000, 1, bidBox);

	KPushButton *bidButton = new KPushButton(i18n("Make Bid"), bidBox, "bidButton");
	connect(bidButton, SIGNAL(clicked()), this, SLOT(slotBidButtonClicked()));

	// Status label
	m_statusLabel = new QLabel(this, "statusLabel");
	m_mainLayout->addWidget(m_statusLabel);
}

void AuctionWidget::auctionChanged()
{
	QString status;
	switch (m_auction->status())
	{
	case 1:
		status = i18n("Going once...");
		break;

	case 2:
		status = i18n("Going twice...");
		break;

	case 3:
		status = i18n("Sold!");
		break;

	default:
		status = QString::null;
	}
	m_statusLabel->setText(status);
}

void AuctionWidget::playerChanged(Player *player)
{
	if (!player)
		return;

	QListViewItem *item;
	if (!(item = m_playerItems[player]))
		return;

	item->setText(0, player->name());
	m_playerList->triggerUpdate();
}

void AuctionWidget::updateBid(Player *player, int amount)
{
	if (!player)
		return;

	QListViewItem *item;
	if (!(item = m_playerItems[player]))
		return;

	item->setText(1, QString::number(amount));
	m_bidSpinBox->setMinValue(amount+1);
	m_playerList->triggerUpdate();
}

void AuctionWidget::slotBidButtonClicked()
{
	emit bid(m_auction, m_bidSpinBox->value());
}
