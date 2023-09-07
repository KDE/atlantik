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

#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QComboBox>

#include <klocalizedstring.h>
#include <kiconloader.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <trade.h>
#include <portfolioestate.h>
#include <card.h>

#include "trade_widget.h"

TradeDisplay::TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent)
	: QDialog(parent, Qt::WindowContextHelpButtonHint)
	, m_atlanticCore(atlanticCore)
	, m_trade(trade)
	, m_contextTradeItem(nullptr)
{
	setWindowTitle(i18nc("@title:window", "Trade %1", trade->tradeId()));

	QVBoxLayout *listCompBox = new QVBoxLayout(this);

	m_updateComponentBox = new QGroupBox(i18n("Add Component"), this);
	listCompBox->addWidget(m_updateComponentBox);

	QHBoxLayout *updateComponentBoxLayout = new QHBoxLayout(m_updateComponentBox);

	m_editTypeCombo = new QComboBox(m_updateComponentBox);
	m_editTypeCombo->addItem(i18n("Estate"));
	m_editTypeCombo->addItem(i18n("Money"));
	m_editTypeCombo->addItem(i18n("Card"));
	updateComponentBoxLayout->addWidget(m_editTypeCombo);

	connect(m_editTypeCombo, SIGNAL(activated(int)), this, SLOT(setTypeCombo(int)));

	m_estateCombo = new QComboBox(m_updateComponentBox);
	updateComponentBoxLayout->addWidget(m_estateCombo);
	foreach (Estate *estate, m_atlanticCore->estates())
	{
		if (estate->isOwned())
		{
			m_estateCombo->addItem( PortfolioEstate::drawPixmap(estate), estate->name() );
			m_estateMap[m_estateCombo->count() - 1] = estate;
			m_estateRevMap[estate] = m_estateCombo->count() - 1;
		}
	}

	connect(m_estateCombo, SIGNAL(activated(int)), this, SLOT(setEstateCombo(int)));

	m_moneyBox = new QSpinBox(m_updateComponentBox);
	m_moneyBox->setRange(0, 10000);
	m_moneyBox->setSingleStep(1);
	updateComponentBoxLayout->addWidget(m_moneyBox);

	m_cardCombo = new QComboBox(m_updateComponentBox);
	updateComponentBoxLayout->addWidget(m_cardCombo);
	foreach (Card *card, m_atlanticCore->cards())
	{
		if (card->isOwned())
		{
			m_cardCombo->addItem(card->title());
			m_cardMap[m_cardCombo->count() - 1] = card;
			m_cardRevMap[card] = m_cardCombo->count() - 1;
		}
	}

	connect(m_cardCombo, SIGNAL(activated(int)), this, SLOT(setCardCombo(int)));

	Player *pSelf = m_atlanticCore->playerSelf();

	m_fromLabel = new QLabel(m_updateComponentBox);
	m_fromLabel->setText(i18n("From"));
	updateComponentBoxLayout->addWidget(m_fromLabel);
	m_playerFromCombo = new QComboBox(m_updateComponentBox);
	updateComponentBoxLayout->addWidget(m_playerFromCombo);

	m_toLabel = new QLabel(m_updateComponentBox);
	m_toLabel->setText(i18n("To"));
	updateComponentBoxLayout->addWidget(m_toLabel);
	m_playerTargetCombo = new QComboBox(m_updateComponentBox);
	updateComponentBoxLayout->addWidget(m_playerTargetCombo);

	foreach (Player *player, m_atlanticCore->players())
	{
		if (player->game() == pSelf->game())
		{
			m_playerFromCombo->addItem(player->name());
			m_playerFromMap[m_playerFromCombo->count() - 1] = player;
			m_playerFromRevMap[player] = m_playerFromCombo->count() - 1;

			m_playerTargetCombo->addItem(player->name());
			m_playerTargetMap[m_playerTargetCombo->count() - 1] = player;
			m_playerTargetRevMap[player] = m_playerTargetCombo->count() - 1;

			connect(player, SIGNAL(changed(Player *)), this, SLOT(playerChanged(Player *)));
		}
	}

	m_updateButton = new QPushButton(i18n("Update"), m_updateComponentBox);
	m_updateButton->setEnabled(false);
	updateComponentBoxLayout->addWidget(m_updateButton);

	connect(m_updateButton, SIGNAL(clicked()), this, SLOT(updateComponent()));

	QHBoxLayout *centralLayout = new QHBoxLayout();
	listCompBox->addLayout(centralLayout);

	m_participantsList = new QTreeWidget(this);
	m_participantsList->setRootIsDecorated(false);
	m_participantsList->header()->setSectionResizeMode(QHeaderView::Stretch);
	m_participantsList->setHeaderLabels(QStringList() << i18n("Participants"));
	QList<QTreeWidgetItem *> items;
	foreach (Player *player, m_trade->participants())
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, player->name());
		m_playerListMap.insert(player, item);
		items << item;
	}
	m_participantsList->addTopLevelItems(items);
	m_participantsList->setMaximumWidth(175); // FIXME grr
	centralLayout->addWidget(m_participantsList);

	m_componentList = new QTreeWidget(this);
        m_componentList->setObjectName(QStringLiteral("componentList"));
	m_componentList->setContextMenuPolicy(Qt::CustomContextMenu);
	m_componentList->setRootIsDecorated(false);
	m_componentList->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	centralLayout->addWidget(m_componentList, 1);

	QStringList headers;
	headers << i18n("Player");
	headers << i18n("Gives");
	headers << i18n("Player");
	headers << i18n("Item");
	m_componentList->setHeaderLabels(headers);

	connect(m_componentList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
	connect(m_componentList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(setCombos(QTreeWidgetItem*)));

	QHBoxLayout *actionBox = new QHBoxLayout();
	actionBox->setContentsMargins(0, 0, 0, 0);
	listCompBox->addItem(actionBox);

	actionBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	m_rejectButton = new QPushButton(KDE::icon(QStringLiteral("dialog-cancel")), i18n("Reject"), this);
	actionBox->addWidget(m_rejectButton);

	connect(m_rejectButton, SIGNAL(clicked()), this, SLOT(reject()));

	m_acceptButton = new QPushButton(KDE::icon(QStringLiteral("dialog-ok")), i18n("Accept"), this);
//	m_acceptButton->setEnabled(false);
	actionBox->addWidget(m_acceptButton);

	connect(m_acceptButton, SIGNAL(clicked()), this, SLOT(accept()));

	m_status = new QLabel(this);
	listCompBox->addWidget(m_status);
	m_status->setText( i18n( "%1 out of %2 players accept current trade proposal.", m_trade->count( true ), m_trade->count( false ) ) );

	connect(m_trade, SIGNAL(itemAdded(TradeItem *)), this, SLOT(tradeItemAdded(TradeItem *)));
	connect(m_trade, SIGNAL(itemRemoved(TradeItem *)), this, SLOT(tradeItemRemoved(TradeItem *)));
	connect(m_trade, SIGNAL(changed(Trade *)), this, SLOT(tradeChanged()));
	connect(m_trade, SIGNAL(rejected(Player *)), this, SLOT(tradeRejected(Player *)));
	connect(m_trade, SIGNAL(playerAdded(Player *)), this, SLOT(slotPlayerAdded(Player *)));
	connect(m_trade, SIGNAL(playerRemoved(Player *)), this, SLOT(slotPlayerRemoved(Player *)));
	connect(m_trade, SIGNAL(acceptChanged(Player *, bool)), this, SLOT(slotAcceptChanged(Player *, bool)));
	connect(this, SIGNAL(updateEstate(Trade *, Estate *, Player *)), m_trade, SIGNAL(updateEstate(Trade *, Estate *, Player *)));
	connect(this, SIGNAL(updateMoney(Trade *, unsigned int, Player *, Player *)), m_trade, SIGNAL(updateMoney(Trade *, unsigned int, Player *, Player *)));
	connect(this, SIGNAL(updateCard(Trade *, Card *, Player *)), m_trade, SIGNAL(updateCard(Trade *, Card *, Player *)));
	connect(this, SIGNAL(reject(Trade *)), m_trade, SIGNAL(reject(Trade *)));
	connect(this, SIGNAL(accept(Trade *)), m_trade, SIGNAL(accept(Trade *)));

	setTypeCombo(m_editTypeCombo->currentIndex());
	setEstateCombo(m_estateCombo->currentIndex());
}

void TradeDisplay::closeEvent(QCloseEvent *e)
{
	// Don't send network event when trade is already rejected
	if (m_trade->isRejected())
		m_atlanticCore->removeTrade(m_trade);
	else
		Q_EMIT reject(m_trade);

	e->accept();
}

void TradeDisplay::tradeItemAdded(TradeItem *tradeItem)
{
	QTreeWidgetItem *item = new QTreeWidgetItem();
	item->setText(0, tradeItem->from() ? tradeItem->from()->name() : QStringLiteral("?"));
	item->setText(1, i18nc("gives is transitive ;)", "gives"));
	item->setText(2, tradeItem->to() ? tradeItem->to()->name() : QStringLiteral("?"));
	item->setText(3, tradeItem->text());
	connect(tradeItem, SIGNAL(changed(TradeItem *)), this, SLOT(tradeItemChanged(TradeItem *)));

	const QIcon userIcon = KDE::icon(QStringLiteral("user-identity"));
	item->setIcon(0, userIcon);
	item->setIcon(2, userIcon);

	if (TradeEstate *tradeEstate = dynamic_cast<TradeEstate*>(tradeItem))
		item->setIcon(3, PortfolioEstate::drawPixmap(tradeEstate->estate()));
//	else if (TradeMoney *tradeMoney = dynamic_cast<TradeMoney*>(tradeItem))
//		item->setIcon(3, PortfolioEstate::pixMap(tradeEstate->estate()));

	m_componentList->addTopLevelItem(item);

	m_componentMap[tradeItem] = item;
	m_componentRevMap[item] = tradeItem;
}

void TradeDisplay::tradeItemRemoved(TradeItem *t)
{
	QTreeWidgetItem *item = m_componentMap.take(t);
	m_componentRevMap.remove(item);
	delete item;
}

void TradeDisplay::tradeItemChanged(TradeItem *t)
{
	QTreeWidgetItem *item = m_componentMap.value(t, nullptr);
	if (item)
	{
		const QIcon userIcon = KDE::icon(QStringLiteral("user-identity"));
		item->setText(0, t->from() ? t->from()->name() : QStringLiteral("?"));
		item->setIcon(0, userIcon);
		item->setText(2, t->to() ? t->to()->name() : QStringLiteral("?"));
		item->setIcon(2, userIcon);
		item->setText(3, t->text());
	}
}

void TradeDisplay::tradeChanged()
{
	// TODO: add notification whether playerSelf has accepted or not and
	// enable/disable accept button based on that
	m_status->setText( i18n( "%1 out of %2 players accept current trade proposal.", m_trade->count( true ), m_trade->count( false ) ) );
}

void TradeDisplay::playerChanged(Player *player)
{
	m_playerFromCombo->setItemText(m_playerFromRevMap[player], player->name());
	m_playerTargetCombo->setItemText(m_playerTargetRevMap[player], player->name());

	foreach (TradeItem *item, m_componentRevMap)
		tradeItemChanged(item);
}

void TradeDisplay::tradeRejected(Player *player)
{
	if (player)
	{
		m_status->setText(i18n("Trade proposal was rejected by %1.", player->name()));
		QTreeWidgetItem *item = m_playerListMap.value(player, nullptr);
		if (item)
			item->setIcon(0, KDE::icon(QStringLiteral("dialog-cancel")));
	} else
		m_status->setText(i18n("Trade proposal was rejected."));

	// Disable GUI elements
	m_updateButton->setEnabled(false);
	m_componentList->setEnabled(false);
	m_rejectButton->setEnabled(false);
	m_acceptButton->setEnabled(false);

	// TODO: add/enable close button
}

void TradeDisplay::slotPlayerAdded(Player *player)
{
	QTreeWidgetItem *item = m_playerListMap.value(player, nullptr);
	if (item)
		return;

	item = new QTreeWidgetItem();
	item->setText(0, player->name());
	m_playerListMap.insert(player, item);
	m_participantsList->addTopLevelItem(item);
	tradeChanged();
}

void TradeDisplay::slotPlayerRemoved(Player *player)
{
	QTreeWidgetItem *item = m_playerListMap.take(player);
	if (!item)
		return;

	delete item;
	tradeChanged();
}

void TradeDisplay::slotAcceptChanged(Player *player, bool accept)
{
	slotPlayerAdded(player);
	QTreeWidgetItem *item = m_playerListMap.value(player, nullptr);
	Q_ASSERT(item);  // slotPlayerAdded made sure there's an item for player
	item->setIcon(0, accept ? KDE::icon(QStringLiteral("dialog-ok")) : QIcon());
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

		m_cardCombo->hide();
		m_cardCombo->setMaximumWidth(0);

		setEstateCombo(m_estateCombo->currentIndex()); // also updates playerfromCombo
		m_playerFromCombo->setEnabled(false);

		m_updateButton->setEnabled( m_estateCombo->count() > 0 );

		break;

	case 1:
		// Editing money component

		m_estateCombo->hide();
		m_estateCombo->setMaximumWidth(0);

		m_moneyBox->show();
		m_moneyBox->setMaximumWidth(9999);

		m_cardCombo->hide();
		m_cardCombo->setMaximumWidth(0);

		m_playerFromCombo->setEnabled(true);

		m_updateButton->setEnabled(true);

		break;

	case 2:
		// Editing card component

		m_estateCombo->hide();
		m_estateCombo->setMaximumWidth(0);

		m_moneyBox->hide();
		m_moneyBox->setMaximumWidth(0);

		m_cardCombo->show();
		m_cardCombo->setMaximumWidth(9999);

		setCardCombo(m_cardCombo->currentIndex()); // also updates playerFromCombo
		m_playerFromCombo->setEnabled(false);

		m_updateButton->setEnabled(m_cardCombo->count() > 0);

		break;
	}
}

void TradeDisplay::setEstateCombo(int index)
{
	if (m_estateCombo->currentIndex() != index)
		m_estateCombo->setCurrentIndex(index);

	if (Estate *estate = m_estateMap.value(index, nullptr))
		m_playerFromCombo->setCurrentIndex( m_playerFromRevMap[estate->owner()] );
}

void TradeDisplay::setCardCombo(int index)
{
	if (m_cardCombo->currentIndex() != index)
		m_cardCombo->setCurrentIndex(index);

	if (Card *card = m_cardMap.value(index, nullptr))
		m_playerFromCombo->setCurrentIndex(m_playerFromRevMap[card->owner()]);
}

void TradeDisplay::setCombos(QTreeWidgetItem *i)
{
	TradeItem *item = m_componentRevMap[i];
	if (TradeEstate *tradeEstate = dynamic_cast<TradeEstate*>(item))
	{
		setTypeCombo(0);
		setEstateCombo( m_estateRevMap[tradeEstate->estate()] ); // also updates playerFromCombo
		m_playerTargetCombo->setCurrentIndex( m_playerTargetRevMap[tradeEstate->to()] );
	}
	else if (TradeMoney *tradeMoney = dynamic_cast<TradeMoney*>(item))
	{
		setTypeCombo(1);
		m_moneyBox->setValue( tradeMoney->money() );
		m_playerFromCombo->setCurrentIndex(  m_playerFromRevMap[tradeMoney->from()] );
		m_playerTargetCombo->setCurrentIndex(  m_playerTargetRevMap[tradeMoney->to()] );
	}
	else if (TradeCard *tradeCard = dynamic_cast<TradeCard*>(item))
	{
		setTypeCombo(2);
		setCardCombo(m_cardRevMap[tradeCard->card()]); // also updates playerFromCombo
		m_playerTargetCombo->setCurrentIndex(m_playerTargetRevMap[tradeCard->to()]);
	}
}

void TradeDisplay::updateComponent()
{
	Estate *estate;
	Player *pFrom, *pTarget;
	Card *card;

	switch (m_editTypeCombo->currentIndex())
	{
	case 0:
		// Updating estate component
		estate = m_estateMap[m_estateCombo->currentIndex()];
		pTarget = m_playerTargetMap[m_playerTargetCombo->currentIndex()];

		if (estate && pTarget)
			Q_EMIT updateEstate(m_trade, estate, pTarget);

		break;

	case 1:
		// Updating money component
		pFrom = m_playerFromMap[m_playerFromCombo->currentIndex()];
		pTarget = m_playerTargetMap[m_playerTargetCombo->currentIndex()];

		if (pFrom && pTarget)
			Q_EMIT updateMoney(m_trade, m_moneyBox->value(), pFrom, pTarget);

		break;

	case 2:
		// Updating card component
		card = m_cardMap[m_cardCombo->currentIndex()];
		pTarget = m_playerTargetMap[m_playerTargetCombo->currentIndex()];

		if (card && pTarget)
			Q_EMIT updateCard(m_trade, card, pTarget);

		break;
	}
}

void TradeDisplay::reject()
{
	Q_EMIT reject(m_trade);
}

void TradeDisplay::accept()
{
	Q_EMIT accept(m_trade);
}

void TradeDisplay::contextMenu(const QPoint &pos)
{
	QTreeWidgetItem* item = m_componentList->itemAt(pos);
	if (!item)
		return;

	m_contextTradeItem = m_componentRevMap[item];

	QMenu *rmbMenu = new QMenu(this);
	QAction *act = rmbMenu->addAction(i18n("Remove From Trade"));
	connect(act, SIGNAL(triggered()), this, SLOT(contextMenuClickedRemove()));

	rmbMenu->exec(m_componentList->viewport()->mapToGlobal(pos));
}

void TradeDisplay::contextMenuClickedRemove()
{
	if (!m_contextTradeItem)
		return;

	if (TradeEstate *tradeEstate = dynamic_cast<TradeEstate*>(m_contextTradeItem))
		Q_EMIT updateEstate(m_trade, tradeEstate->estate(), nullptr);
	else if (TradeMoney *tradeMoney = dynamic_cast<TradeMoney*>(m_contextTradeItem))
		Q_EMIT updateMoney(m_trade, 0, tradeMoney->from(), tradeMoney->to());
	else if (TradeCard *tradeCard = dynamic_cast<TradeCard*>(m_contextTradeItem))
		Q_EMIT updateCard(m_trade, tradeCard->card(), nullptr);

	m_contextTradeItem = nullptr;
}

#include "moc_trade_widget.cpp"
