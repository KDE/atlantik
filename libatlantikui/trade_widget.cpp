#include <qlayout.h>
#include <qhgroupbox.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qvalidator.h>
#include <qmap.h>
#include <qlabel.h>

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

class PlayerListViewItem : public QListViewItem
{
public:
	PlayerListViewItem(KListView *parent, Player *p, const QString &str)
		: QListViewItem(parent, str), mPlayer(p)
	{}
	
	Player *player() { return mPlayer; }
	
private:
	Player *mPlayer;
};

class GivingListViewItem : public QListViewItem
{
public:
	GivingListViewItem(PlayerListViewItem *parent, Player *p, const QString &str)
		: QListViewItem(parent, str), mPlayer(p)
	{}
	
	Player *player() { return mPlayer; }
	
	PlayerListViewItem *parent()
		{ return static_cast<PlayerListViewItem*>(QListViewItem::parent()); }
	
private:
	Player *mPlayer;
};

class TradeListViewItem : public QListViewItem
{
public:
	TradeListViewItem(GivingListViewItem *parent, TradeItem *t, const QString &str)
		: QListViewItem(parent, str), mTrade(t)
	{}
	
	TradeItem *trade() { return mTrade; }
	
private:
	TradeItem *mTrade;
};

TradeDisplay::TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_trade = trade;
	m_atlanticCore = atlanticCore;

	QVBoxLayout *listCompBox = new QVBoxLayout(this, 11, 6);

	QHGroupBox *addEstateBox = new QHGroupBox(i18n("Add component"), this);
	listCompBox->addWidget(addEstateBox);

	KComboBox *typeBox = new KComboBox(addEstateBox);
	typeBox->insertItem(i18n("Estate"));
//	typeBox->insertItem(i18n("Money"));

	m_estateCombo = new KComboBox(addEstateBox);
	QPtrList<Estate> estateList = m_atlanticCore->estates();
	Estate *estate;
	for (QPtrListIterator<Estate> it(estateList); *it; ++it)
	{
		if ((estate = *it) && estate->isOwned())
		{
			// TODO: map estate pointer to itemid for reverse lookup
			m_estateCombo->insertItem(estate->name());
		}
	}

	m_playerCombo = new KComboBox(addEstateBox);
	QPtrList<Player> playerList = m_atlanticCore->players();
	Player *player;
	for (QPtrListIterator<Player> it(playerList); *it; ++it)
	{
		if ((player = *it))
		{
			// TODO: map player pointer to itemid for reverse lookup
			m_playerCombo->insertItem(player->name());
		}
	}

	KPushButton *addEstateButton = new KPushButton(i18n("Add estate"), addEstateBox);
//	addEstateButton->setEnabled(false);

	m_componentList = new KListView(this, "componentList");
	listCompBox->addWidget(m_componentList);

	m_componentList->addColumn("Player");
	m_componentList->addColumn("gives");
	m_componentList->addColumn("Player");
	m_componentList->addColumn("Item");

	m_status = new QLabel(this);
	listCompBox->addWidget(m_status);
	m_status->setText(i18n("%1 out of %2 players accept current trade proposal.").arg(0).arg(0));

//	mPlayerList->header()->hide();
//	mPlayerList->setRootIsDecorated(true);
//	mPlayerList->setResizeMode(KListView::AllColumns);
	
	connect(trade, SIGNAL(playerAdded(Player *)), this, SLOT(playerAdded(Player *)));
	connect(trade, SIGNAL(playerRemoved(Player *)), this, SLOT(playerRemoved(Player *)));
	connect(trade, SIGNAL(tradeAdded(TradeItem *)), this, SLOT(tradeAdded(TradeItem *)));
	connect(trade, SIGNAL(tradeRemoved(TradeItem *)), this, SLOT(tradeRemoved(TradeItem *)));
	connect(trade, SIGNAL(tradeChanged(TradeItem *)), this, SLOT(tradeChanged(TradeItem *)));

	connect(addEstateButton, SIGNAL(clicked()), this, SLOT(updateEstate()));

//	connect(
//			mPlayerList,
//			SIGNAL(contextMenu(KListView*, QListViewItem *, const QPoint&)),
//			SLOT(contextMenu(KListView *, QListViewItem *)));
}

void TradeDisplay::playerAdded(Player *p)
{
/*
	PlayerListViewItem *other=0;
	for (QListViewItem *i=mPlayerList->firstChild() ; i; i=i->nextSibling())
	{
		// add myself to the other toplevels
		GivingListViewItem *othergiver=0;
		othergiver = new GivingListViewItem(
				static_cast<PlayerListViewItem*>(i), p,
				i18n("gives is transitive ;)", "Gives %1").arg(p->name())
			);
		other = othergiver->parent();
	}
	
	PlayerListViewItem *item=new PlayerListViewItem(mPlayerList, p, p->name());
	
	QPtrList<Player> players=trade()->players();
	
	for (QPtrListIterator<Player> i(players); *i; ++i)
	{
		// add "Gives someone" to the other toplevels
		PlayerListViewItem *top = player(*i);
		if (top == other) continue; // don't add me to me
		
		new GivingListViewItem(
				top, p,
				i18n("gives is transitive ;)", "Gives %1").arg(p->name())
			);
		
	}
*/	
}

void TradeDisplay::playerRemoved(Player *p)
{
/*
	delete player(p);
*/
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

void TradeDisplay::updateEstate()
{
	QPtrList<Estate> estateList = m_atlanticCore->estates();
	Estate *estate = 0;
	for (QPtrListIterator<Estate> it(estateList); *it; ++it)
	{
		if ((estate = *it) && estate->name() == m_estateCombo->currentText())
			break;
		estate = 0;
	}
	QPtrList<Player> playerList = m_atlanticCore->players();
	Player *player = 0;
	for (QPtrListIterator<Player> it(playerList); *it; ++it)
	{
		if ((player = *it) && player->name() == m_playerCombo->currentText())
			break;
		player = 0;
	}

	if (estate && player)
		emit updateEstate(m_trade, estate, player);
}

void TradeDisplay::contextMenu(KListView *l, QListViewItem *item)
{
	if (dynamic_cast<TradeListViewItem*>(item))
	{
		TradeListViewItem *i=static_cast<TradeListViewItem*>(item);
	
		QPopupMenu menu(this);
		menu.insertItem(i18n("&Remove Trade"), 0);
		
		if (dynamic_cast<TradeMoney*>(i->trade()))
			menu.insertItem(i18n("&Change Value"), 1);
		
		switch (menu.exec())
		{
		case 0:
			trade()->removeTradeItem(i->trade());
			break;
		case 1:
			{
				KDialogBase dlg(
						this, 0, true,
						i18n("Trade Money"),
						KDialogBase::Ok|KDialogBase::Cancel
					);
				KLineEdit *e=new KLineEdit(&dlg);
				dlg.setMainWidget(e);
				e->setValidator(
						new QIntValidator(0, i->trade()->from()->money(), this)
					);
				
				int result=dlg.exec();
				if (result==KDialogBase::Ok)
				{
					// TODO money changed
				}
			}
		}
	}
	else if (dynamic_cast<GivingListViewItem*>(item))
	{
		GivingListViewItem *i=static_cast<GivingListViewItem*>(item);
		QPopupMenu menu(this);
		menu.insertItem(i18n("Trade &Money"), 0);

		QPtrList<Estate> estateList = m_atlanticCore->estates();
		QMap<int, Estate*> id;
		QPopupMenu estates(this);
		
		{
			int num=1;
			for (QPtrListIterator<Estate> it(estateList); *it; ++it)
			{
				if ((*it)->owner()==i->parent()->player())
				{
					id[num]=*it;
					estates.insertItem((*it)->name(), num);
					num++;
				}
			}
		}
		
		menu.insertItem(i18n("Trade &Estate"), &estates);
		
		int result=menu.exec(QCursor::pos());
	
		if (result!=-1)
		{
			TradeEstate *t=
					new TradeEstate(id[result], mTrade, i->parent()->player());
			mTrade->addTradeItem(t);
		}
	}
}
