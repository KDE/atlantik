#include <qlayout.h>
#include <qheader.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qvalidator.h>

#include <klocale.h>
#include <klistview.h>
#include <kdebug.h>
#include <kdialogbase.h>
#include <klineedit.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <trade.h>

#include "trade_widget.moc"

class PlayerListViewItem : public QListViewItem
{
public:
	PlayerListViewItem(QListView *parent, Player *p, const QString &str)
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

TradeDisplay::TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent, const char *name)
	: QWidget(parent, name), mTrade(trade)
{
	m_atlanticCore = atlanticCore;

	(new QHBoxLayout(this, 11, 6))->setAutoAdd(true);
	
	mPlayerList = new KListView(this, "playerlist");
	mPlayerList->header()->hide();
	mPlayerList->addColumn("nutin'!");
	mPlayerList->setRootIsDecorated(true);
	mPlayerList->setResizeMode(QListView::AllColumns);
	
	
	connect(trade, SIGNAL(playerAdded(Player *)),
			SLOT(playerAdded(Player *))
		);
	connect(trade, SIGNAL(playerRemoved(Player *)),
			SLOT(playerRemoved(Player *))
		);

	connect(trade, SIGNAL(tradeAdded(TradeItem *)),
			SLOT(tradeAdded(TradeItem *))
		);
	connect(trade, SIGNAL(tradeRemoved(TradeItem *)),
			SLOT(tradeRemoved(TradeItem *))
		);
	connect(trade, SIGNAL(tradeChanged(TradeItem *)),
			SLOT(tradeChanged(TradeItem *))
		);
	connect(
			mPlayerList,
			SIGNAL(contextMenu(KListView*, QListViewItem *, const QPoint&)),
			SLOT(contextMenu(KListView *, QListViewItem *))
		);
}


GivingListViewItem *TradeDisplay::giving(Player *from, Player *to)
{
	PlayerListViewItem *fromItem=player(from);
	if (!fromItem) return 0;
	
	for (
			GivingListViewItem *i=
					static_cast<GivingListViewItem*>(fromItem->firstChild());
			i;
			i=static_cast<GivingListViewItem*>(i->nextSibling()))
	{
		if (i->player()==to)
			return i;
	}
	return 0;
}

GivingListViewItem *TradeDisplay::giving(TradeItem *t)
{
	return giving(t->from(), t->to());
}

PlayerListViewItem *TradeDisplay::player(Player *from)
{
	for (
			PlayerListViewItem *i=
					static_cast<PlayerListViewItem*>(mPlayerList->firstChild());
			i;
			i=static_cast<PlayerListViewItem*>(i->nextSibling()))
	{
		if (i->player()==from)
			return i;
	}
	
	return 0;
}

TradeListViewItem  *TradeDisplay::trade(TradeItem *t)
{
	GivingListViewItem *g=giving(t);
	if (!g) return 0;
	
	for (
			TradeListViewItem *i=
					static_cast<TradeListViewItem*>(g->firstChild());
			i;
			i=static_cast<TradeListViewItem*>(i->nextSibling()))
	{
		if (i->trade()==t)
			return i;
	}
	return 0;
}


void TradeDisplay::playerAdded(Player *p)
{
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
	
}

void TradeDisplay::playerRemoved(Player *p)
{
	delete player(p);
}

void TradeDisplay::tradeAdded(TradeItem *t)
{
	new TradeListViewItem(giving(t), t, t->text());
}
	
void TradeDisplay::tradeRemoved(TradeItem *t)
{
	delete trade(t);
}

void TradeDisplay::tradeChanged(TradeItem *t)
{
	delete trade(t);
	tradeAdded(t);
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



