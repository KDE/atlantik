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

#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QTreeWidget>
#include <QPushButton>

#include <kiconloader.h>
#include <klocalizedstring.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <auction.h>

#include "auction_widget.h"

#include <ui_auction.h>

AuctionWidget::AuctionWidget(AtlanticCore *atlanticCore, Auction *auction, QWidget *parent) : QWidget(parent)
{
	m_atlanticCore = atlanticCore;

	m_auction = auction;
	connect(m_auction, SIGNAL(changed()), this, SLOT(auctionChanged()));
	connect(m_auction, SIGNAL(updateBid(Player *, int)), this, SLOT(updateBid(Player *, int)));
	connect(this, SIGNAL(bid(Auction *, int)), m_auction, SIGNAL(bid(Auction *, int)));

	m_ui = new Ui::AuctionWidget();
	m_ui->setupUi(this);

	// Player list
	Estate *estate = auction->estate();
	m_ui->playerGroupBox->setTitle(estate ? i18n("Auction: %1", estate->name()) : i18n("Auction"));

	m_ui->playerList->header()->setSectionsClickable(false);
	m_ui->playerList->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	QList<QTreeWidgetItem *> items;
	Player *pSelf = m_atlanticCore->playerSelf();

	foreach (Player *player, m_atlanticCore->players())
	{
		if (player->game() == pSelf->game())
		{
			QTreeWidgetItem *item = new QTreeWidgetItem();
			item->setText(0, player->name());
			item->setText(1, QString("0"));
			item->setIcon(0, KDE::icon("user-identity"));
			m_playerItems[player] = item;
			items << item;

			connect(player, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
		}
	}
	m_ui->playerList->addTopLevelItems(items);

	// Bid spinbox and button
	connect(m_ui->bidButton, SIGNAL(clicked()), this, SLOT(slotBidButtonClicked()));
}

AuctionWidget::~AuctionWidget()
{
	delete m_ui;
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
		status.clear();
	}
	m_ui->statusLabel->setText(status);
}

void AuctionWidget::playerChanged(Player *player)
{
	if (!player)
		return;

	QTreeWidgetItem *item;
	if (!(item = m_playerItems.value(player, 0)))
		return;

	item->setText(0, player->name());
}

void AuctionWidget::updateBid(Player *player, int amount)
{
	if (!player)
		return;

	QTreeWidgetItem *item;
	if (!(item = m_playerItems.value(player, 0)))
		return;

	item->setText(1, QString::number(amount));
	m_ui->bidSpinBox->setMinimum(amount+1);
}

void AuctionWidget::slotBidButtonClicked()
{
	emit bid(m_auction, m_ui->bidSpinBox->value());
}
