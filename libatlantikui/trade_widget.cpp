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

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <trade.h>

#include "trade_widget.moc"

TradeDisplay::TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_trade = trade;
	m_atlanticCore = atlanticCore;

	QVBoxLayout *listCompBox = new QVBoxLayout(this, 11, 6);

	QHGroupBox *addEstateBox = new QHGroupBox(i18n("Add component"), this);
	listCompBox->addWidget(addEstateBox);

	m_editTypeCombo = new KComboBox(addEstateBox);
	m_editTypeCombo->insertItem(i18n("Estate"));
	m_editTypeCombo->insertItem(i18n("Money"));

	connect(m_editTypeCombo, SIGNAL(activated(int)), this, SLOT(setEditType(int)));

	m_estateCombo = new KComboBox(addEstateBox);
	QPtrList<Estate> estateList = m_atlanticCore->estates();
	Estate *estate;
	for (QPtrListIterator<Estate> it(estateList); *it; ++it)
	{
		if ((estate = *it) && estate->isOwned())
		{
			m_estateCombo->insertItem(estate->name());
			m_estateMap[m_estateCombo->count() - 1] = estate;
		}
	}

	connect(m_estateCombo, SIGNAL(activated(int)), this, SLOT(setEditEstate(int)));

	m_moneyBox = new QSpinBox(0, 10000, 1, addEstateBox);

	QPtrList<Player> playerList = m_atlanticCore->players();
	Player *player;

	m_fromLabel = new QLabel(addEstateBox);
	m_fromLabel->setText(i18n("From"));

	m_playerFromCombo = new KComboBox(addEstateBox);
	for (QPtrListIterator<Player> it(playerList); *it; ++it)
	{
		if ((player = *it))
		{
			m_playerFromCombo->insertItem(player->name());
			m_playerFromMap[m_playerFromCombo->count() - 1] = player;
			m_playerFromRevMap[player] = m_playerFromCombo->count() - 1;
		}
	}

	m_toLabel = new QLabel(addEstateBox);
	m_toLabel->setText(i18n("To"));

	m_playerTargetCombo = new KComboBox(addEstateBox);
	for (QPtrListIterator<Player> it(playerList); *it; ++it)
	{
		if ((player = *it))
		{
			m_playerTargetCombo->insertItem(player->name());
			m_playerTargetMap[m_playerTargetCombo->count() - 1] = player;
			m_playerTargetRevMap[player] = m_playerTargetCombo->count() - 1;
		}
	}

	m_updateButton = new KPushButton(i18n("Update"), addEstateBox);
	m_updateButton->setEnabled(false);

	connect(m_updateButton, SIGNAL(clicked()), this, SLOT(updateComponent()));

	m_componentList = new KListView(this, "componentList");
	listCompBox->addWidget(m_componentList);

	m_componentList->addColumn("Player");
	m_componentList->addColumn("gives");
	m_componentList->addColumn("Player");
	m_componentList->addColumn("Item");

	QHBoxLayout *actionBox = new QHBoxLayout(this);
	listCompBox->addItem(actionBox);

	KPushButton *rejectButton = new KPushButton(i18n("Reject"), this);
	actionBox->addWidget(rejectButton);

	connect(rejectButton, SIGNAL(clicked()), this, SLOT(reject()));

	KPushButton *acceptButton = new KPushButton(i18n("Accept"), this);
	acceptButton->setEnabled(false);
	actionBox->addWidget(acceptButton);

	m_status = new QLabel(this);
	listCompBox->addWidget(m_status);
	m_status->setText(i18n("%1 out of %2 players accept current trade proposal.").arg(0).arg(0));

//	mPlayerList->header()->hide();
//	mPlayerList->setRootIsDecorated(true);
//	mPlayerList->setResizeMode(KListView::AllColumns);
	
	connect(trade, SIGNAL(tradeAdded(TradeItem *)), this, SLOT(tradeAdded(TradeItem *)));
	connect(trade, SIGNAL(tradeRemoved(TradeItem *)), this, SLOT(tradeRemoved(TradeItem *)));
	connect(trade, SIGNAL(tradeChanged(TradeItem *)), this, SLOT(tradeChanged(TradeItem *)));

	setEditType(m_editTypeCombo->currentItem());
	setEditEstate(m_estateCombo->currentItem());

	connect(m_componentList, SIGNAL(contextMenu(KListView*, QListViewItem *, const QPoint&)), SLOT(contextMenu(KListView *, QListViewItem *)));
}

void TradeDisplay::closeEvent(QCloseEvent *e)
{
	emit reject(m_trade);
	e->accept();
}

void TradeDisplay::tradeAdded(TradeItem *t)
{
	KListViewItem *item = new KListViewItem(m_componentList, (t->from() ? t->from()->name() : QString("?")), i18n("gives is transitive ;)", "gives"), (t->to() ? t->to()->name() : QString("?")), t->text());
	item->setPixmap(0, QPixmap(SmallIcon("personal")));
	item->setPixmap(2, QPixmap(SmallIcon("personal")));

	m_componentMap[t] = item;
}
	
void TradeDisplay::tradeRemoved(TradeItem *t)
{
	KListViewItem *item = m_componentMap[t];
	delete item;
//	delete trade(t);
}

void TradeDisplay::tradeChanged(TradeItem *t)
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

void TradeDisplay::setEditType(int index)
{
	switch (index)
	{
	case 0:
		// Editing estate component

		m_estateCombo->show();
		m_estateCombo->setMaximumWidth(9999);

		m_moneyBox->hide();
		m_moneyBox->setMaximumWidth(0);

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

void TradeDisplay::setEditEstate(int index)
{
	if (Estate *estate = m_estateMap[index])
		m_playerFromCombo->setCurrentItem( m_playerFromRevMap[estate->owner()] );
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

void TradeDisplay::contextMenu(KListView *l, QListViewItem *item)
{
	// TODO: Add simple menu to remove item
}
