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

#include <iostream>

#include <qlayout.h>
#include <qhgroupbox.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qvalidator.h>
#include <qmap.h>
#include <qlabel.h>
#include <qspinbox.h>

#include <klocale.h>
#include <klistview.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kcombobox.h>
#include <kpopupmenu.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <trade.h>
#include <portfolioestate.h>

#include "trade_widget.moc"

TradeDisplay::TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent, const char *name)
	: QWidget(parent, name,
	  WType_Dialog | WStyle_Customize | WStyle_DialogBorder | WStyle_Title |
	  WStyle_Minimize | WStyle_ContextHelp )
{
	m_trade = trade;
	m_atlanticCore = atlanticCore;

	setCaption(i18n("Trade %1").arg(trade->tradeId()));

	QVBoxLayout *listCompBox = new QVBoxLayout(this, KDialog::marginHint());

	m_updateComponentBox = new QHGroupBox(i18n("Add Component"), this);
	listCompBox->addWidget(m_updateComponentBox);

	m_editTypeCombo = new KComboBox(m_updateComponentBox);
	m_editTypeCombo->insertItem(i18n("Estate"));
	m_editTypeCombo->insertItem(i18n("Money"));

	connect(m_editTypeCombo, SIGNAL(activated(int)), this, SLOT(setTypeCombo(int)));

	m_estateCombo = new KComboBox(m_updateComponentBox);
	QPtrList<Estate> estateList = m_atlanticCore->estates();
	Estate *estate;
	for (QPtrListIterator<Estate> it(estateList); *it; ++it)
	{
		if ((estate = *it) && estate->isOwned())
		{
			m_estateCombo->insertItem( PortfolioEstate::drawPixmap(estate), estate->name() );
			m_estateMap[m_estateCombo->count() - 1] = estate;
			m_estateRevMap[estate] = m_estateCombo->count() - 1;
		}
	}

	connect(m_estateCombo, SIGNAL(activated(int)), this, SLOT(setEstateCombo(int)));

	m_moneyBox = new QSpinBox(0, 10000, 1, m_updateComponentBox);

	QPtrList<Player> playerList = m_atlanticCore->players();
	Player *player, *pSelf = m_atlanticCore->playerSelf();

	m_fromLabel = new QLabel(m_updateComponentBox);
	m_fromLabel->setText(i18n("From"));
	m_playerFromCombo = new KComboBox(m_updateComponentBox);

	m_toLabel = new QLabel(m_updateComponentBox);
	m_toLabel->setText(i18n("To"));
	m_playerTargetCombo = new KComboBox(m_updateComponentBox);

	for (QPtrListIterator<Player> it(playerList); *it; ++it)
	{
		if ((player = *it) && player->game() == pSelf->game())
		{
			m_playerFromCombo->insertItem(player->name());
			m_playerFromMap[m_playerFromCombo->count() - 1] = player;
			m_playerFromRevMap[player] = m_playerFromCombo->count() - 1;

			m_playerTargetCombo->insertItem(player->name());
			m_playerTargetMap[m_playerTargetCombo->count() - 1] = player;
			m_playerTargetRevMap[player] = m_playerTargetCombo->count() - 1;

			connect(player, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
		}
	}

	m_updateButton = new KPushButton(i18n("Update"), m_updateComponentBox);
	m_updateButton->setEnabled(false);

	connect(m_updateButton, SIGNAL(clicked()), this, SLOT(updateComponent()));

	m_componentList = new KListView(this, "componentList");
	listCompBox->addWidget(m_componentList);

	m_componentList->addColumn(i18n("Player"));
	m_componentList->addColumn(i18n("Gives"));
	m_componentList->addColumn(i18n("Player"));
	m_componentList->addColumn(i18n("Item"));

	connect(m_componentList, SIGNAL(contextMenu(KListView*, QListViewItem *, const QPoint&)), SLOT(contextMenu(KListView *, QListViewItem *, const QPoint&)));
	connect(m_componentList, SIGNAL(clicked(QListViewItem *)), this, SLOT(setCombos(QListViewItem *)));

	QHBoxLayout *actionBox = new QHBoxLayout(this, 0, KDialog::spacingHint());
	listCompBox->addItem(actionBox);

	actionBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_rejectButton = new KPushButton(BarIcon("cancel", KIcon::SizeSmall), i18n("Reject"), this);
	actionBox->addWidget(m_rejectButton);

	connect(m_rejectButton, SIGNAL(clicked()), this, SLOT(reject()));

	m_acceptButton = new KPushButton(BarIcon("ok", KIcon::SizeSmall), i18n("Accept"), this);
//	m_acceptButton->setEnabled(false);
	actionBox->addWidget(m_acceptButton);

	connect(m_acceptButton, SIGNAL(clicked()), this, SLOT(accept()));

	m_status = new QLabel(this);
	listCompBox->addWidget(m_status);
	m_status->setText( i18n( "%1 out of %2 players accept current trade proposal." ).arg( m_trade->count( true ) ).arg( m_trade->count( false ) ) );

//	mPlayerList->header()->hide();
//	mPlayerList->setRootIsDecorated(true);
//	mPlayerList->setResizeMode(KListView::AllColumns);

	connect(m_trade, SIGNAL(itemAdded(TradeItem *)), this, SLOT(tradeItemAdded(TradeItem *)));
	connect(m_trade, SIGNAL(itemRemoved(TradeItem *)), this, SLOT(tradeItemRemoved(TradeItem *)));
	connect(m_trade, SIGNAL(changed(Trade *)), this, SLOT(tradeChanged()));
	connect(m_trade, SIGNAL(rejected(Player *)), this, SLOT(tradeRejected(Player *)));
	connect(this, SIGNAL(updateEstate(Trade *, Estate *, Player *)), m_trade, SIGNAL(updateEstate(Trade *, Estate *, Player *)));
	connect(this, SIGNAL(updateMoney(Trade *, unsigned int, Player *, Player *)), m_trade, SIGNAL(updateMoney(Trade *, unsigned int, Player *, Player *)));
	connect(this, SIGNAL(reject(Trade *)), m_trade, SIGNAL(reject(Trade *)));
	connect(this, SIGNAL(accept(Trade *)), m_trade, SIGNAL(accept(Trade *)));

	setTypeCombo(m_editTypeCombo->currentItem());
	setEstateCombo(m_estateCombo->currentItem());

	m_contextTradeItem = 0;
}

void TradeDisplay::closeEvent(QCloseEvent *e)
{
	// Don't send network event when trade is already rejected
	if (m_trade->isRejected())
		m_atlanticCore->removeTrade(m_trade);
	else
		emit reject(m_trade);

	e->accept();
}

void TradeDisplay::tradeItemAdded(TradeItem *tradeItem)
{
	KListViewItem *item = new KListViewItem(m_componentList, (tradeItem->from() ? tradeItem->from()->name() : QString("?")), i18n("gives is transitive ;)", "gives"), (tradeItem->to() ? tradeItem->to()->name() : QString("?")), tradeItem->text());
	connect(tradeItem, SIGNAL(changed(TradeItem *)), this, SLOT(tradeItemChanged(TradeItem *)));

	item->setPixmap(0, QPixmap(SmallIcon("personal")));
	item->setPixmap(2, QPixmap(SmallIcon("personal")));

	if (TradeEstate *tradeEstate = dynamic_cast<TradeEstate*>(tradeItem))
		item->setPixmap(3, PortfolioEstate::drawPixmap(tradeEstate->estate()));
//	else if (TradeMoney *tradeMoney = dynamic_cast<TradeMoney*>(tradeItem))
//		item->setPixmap(3, PortfolioEstate::pixMap(tradeEstate->estate()));

	m_componentMap[tradeItem] = item;
	m_componentRevMap[item] = tradeItem;
}

void TradeDisplay::tradeItemRemoved(TradeItem *t)
{
	KListViewItem *item = m_componentMap[t];
	delete item;
	m_componentMap[t] = 0;
}

void TradeDisplay::tradeItemChanged(TradeItem *t)
{
	KListViewItem *item = m_componentMap[t];
	if (item)
	{
		item->setText(0, t->from() ? t->from()->name() : QString("?"));
		item->setPixmap(0, QPixmap(SmallIcon("personal")));
		item->setText(2, t->to() ? t->to()->name() : QString("?"));
		item->setPixmap(2, QPixmap(SmallIcon("personal")));
		item->setText(3, t->text());
	}
}

void TradeDisplay::tradeChanged()
{
	// TODO: add notification whether playerSelf has accepted or not and
	// enable/disable accept button based on that
	m_status->setText( i18n( "%1 out of %2 players accept current trade proposal." ).arg( m_trade->count( true ) ).arg( m_trade->count( false ) ) );
}

void TradeDisplay::playerChanged(Player *player)
{
	m_playerFromCombo->changeItem(player->name(), m_playerFromRevMap[player]);
	m_playerTargetCombo->changeItem(player->name(), m_playerTargetRevMap[player]);

	TradeItem *item = 0;
	for (QMap<KListViewItem *, TradeItem *>::Iterator it=m_componentRevMap.begin() ; it != m_componentRevMap.end() && (item = *it) ; ++it)
		tradeItemChanged(item);
}

void TradeDisplay::tradeRejected(Player *player)
{
	if (player)
		m_status->setText(i18n("Trade proposal was rejected by %1.").arg(player->name()));
	else
		m_status->setText(i18n("Trade proposal was rejected."));

	// Disable GUI elements
	m_updateButton->setEnabled(false);
	m_componentList->setEnabled(false);
	m_rejectButton->setEnabled(false);
	m_acceptButton->setEnabled(false);

	// TODO: add/enable close button
}

void TradeDisplay::setTypeCombo(int index)
{
	switch (index)
	{
	case 0:
		// Editing estate component

		m_estateCombo->show();
		m_estateCombo->setMaximumWidth(9999);

		m_moneyBox->hide();
		m_moneyBox->setMaximumWidth(0);

		setEstateCombo(m_estateCombo->currentItem()); // also updates playerfromCombo
		m_playerFromCombo->setEnabled(false);

		m_updateButton->setEnabled( m_estateCombo->count() > 0 );

		break;

	case 1:
		// Editing money component

		m_estateCombo->hide();
		m_estateCombo->setMaximumWidth(0);

		m_moneyBox->show();
		m_moneyBox->setMaximumWidth(9999);

		m_playerFromCombo->setEnabled(true);

		m_updateButton->setEnabled(true);

		break;
	}
}

void TradeDisplay::setEstateCombo(int index)
{
	if (m_estateCombo->currentItem() != index)
		m_estateCombo->setCurrentItem(index);

	if (Estate *estate = m_estateMap[index])
		m_playerFromCombo->setCurrentItem( m_playerFromRevMap[estate->owner()] );
}

void TradeDisplay::setCombos(QListViewItem *i)
{
	TradeItem *item = m_componentRevMap[(KListViewItem *)(i)];
	if (TradeEstate *tradeEstate = dynamic_cast<TradeEstate*>(item))
	{
		setTypeCombo(0);
		setEstateCombo( m_estateRevMap[tradeEstate->estate()] ); // also updates playerFromCombo
		m_playerTargetCombo->setCurrentItem( m_playerTargetRevMap[tradeEstate->to()] );
	}
	else if (TradeMoney *tradeMoney = dynamic_cast<TradeMoney*>(item))
	{
		setTypeCombo(1);
		m_moneyBox->setValue( tradeMoney->money() );
		m_playerFromCombo->setCurrentItem(  m_playerFromRevMap[tradeMoney->from()] );
		m_playerTargetCombo->setCurrentItem(  m_playerTargetRevMap[tradeMoney->to()] );
	}
}

void TradeDisplay::updateComponent()
{
	Estate *estate;
	Player *pFrom, *pTarget;

	switch (m_editTypeCombo->currentItem())
	{
	case 0:
		// Updating estate component
		estate = m_estateMap[m_estateCombo->currentItem()];
		pTarget = m_playerTargetMap[m_playerTargetCombo->currentItem()];

		if (estate && pTarget)
			emit updateEstate(m_trade, estate, pTarget);

		break;

	case 1:
		// Updating money component
		pFrom = m_playerFromMap[m_playerFromCombo->currentItem()];
		pTarget = m_playerTargetMap[m_playerTargetCombo->currentItem()];

		if (pFrom && pTarget)
			emit updateMoney(m_trade, m_moneyBox->value(), pFrom, pTarget);

		break;
	}
}

void TradeDisplay::reject()
{
	emit reject(m_trade);
}

void TradeDisplay::accept()
{
	emit accept(m_trade);
}

void TradeDisplay::contextMenu(KListView *, QListViewItem *i, const QPoint& p)
{
	m_contextTradeItem = m_componentRevMap[(KListViewItem *)(i)];

	KPopupMenu *rmbMenu = new KPopupMenu(this);
//	rmbMenu->insertTitle( ... );
	rmbMenu->insertItem(i18n("Remove From Trade"), 0);

	connect(rmbMenu, SIGNAL(activated(int)), this, SLOT(contextMenuClicked(int)));
	rmbMenu->exec(p);
}

void TradeDisplay::contextMenuClicked(int)
{
	if (!m_contextTradeItem)
		return;

	if (TradeEstate *tradeEstate = dynamic_cast<TradeEstate*>(m_contextTradeItem))
		emit updateEstate(m_trade, tradeEstate->estate(), 0);
	else if (TradeMoney *tradeMoney = dynamic_cast<TradeMoney*>(m_contextTradeItem))
		emit updateMoney(m_trade, 0, tradeMoney->from(), tradeMoney->to());

	m_contextTradeItem = 0;
}
