#include <qlayout.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qvalidator.h>
#include <qmap.h>

#include <klocale.h>
#include <klistview.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <klineedit.h>
#include <kiconloader.h>

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

	(new QHBoxLayout(this, 11, 6))->setAutoAdd(true);

	m_componentList = new KListView(this, "componentList");
	m_componentList->addColumn("Player");
	m_componentList->addColumn("gives");
	m_componentList->addColumn("Player");
	m_componentList->addColumn("Item");

//	mPlayerList->header()->hide();
//	mPlayerList->setRootIsDecorated(true);
//	mPlayerList->setResizeMode(KListView::AllColumns);
	
	connect(trade, SIGNAL(playerAdded(Player *)), this, SLOT(playerAdded(Player *)));
	connect(trade, SIGNAL(playerRemoved(Player *)), this, SLOT(playerRemoved(Player *)));
	connect(trade, SIGNAL(tradeAdded(TradeItem *)), this, SLOT(tradeAdded(TradeItem *)));
	connect(trade, SIGNAL(tradeRemoved(TradeItem *)), this, SLOT(tradeRemoved(TradeItem *)));
	connect(trade, SIGNAL(tradeChanged(TradeItem *)), this, SLOT(tradeChanged(TradeItem *)));

	return;

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
	KListViewItem *item = new KListViewItem(m_componentList, t->from()->name(), i18n("gives is transitive ;)", "gives"), t->to()->name(), t->text());
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
		item->setText(0, t->from()->name());
		item->setPixmap(0, QPixmap(SmallIcon("personal")));
		item->setText(2, t->to()->name());
		item->setPixmap(2, QPixmap(SmallIcon("personal")));
		item->setText(3, t->text());
	}
}

void TradeDisplay::contextMenu(KListView *l, QListViewItem *item)
{
	if (dynamic_cast<TradeListViewItem*>(item))
	{
		TradeListViewItem *i=static_cast<TradeListViewItem*>(item);
	
		QPopupMenu menu(this);
		menu.insertItem(i18n("&Remove Trade"), 0);
		
		if (dynamic_cast<TradeCash*>(i->trade()))
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
