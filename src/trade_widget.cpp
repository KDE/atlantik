#include <klocale.h>
#include <klistview.h>
#include <qhbox.h>
#include <kdebug.h>
#include <qheader.h>
#include "trade.h"
#include "trade_widget.moc"
#include "player.h"

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

TradeDisplay::TradeDisplay(Trade *trade, QWidget *parent, const char *name)
	: QWidget(parent, name), mTrade(trade)
{
	QHBox *main=new QHBox(this);
	main->setSpacing(6);
	
	mPlayerList = new KListView(main, "playerlist");
	mPlayerList->header()->hide();
	mPlayerList->addColumn("nutin'!");
	
	
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
	for (QListViewItem *i=mPlayerList->firstChild() ; i; i=i->nextSibling())
	{	
		new GivingListViewItem(
				static_cast<PlayerListViewItem*>(i), p, 
				i18n("Someone gives person %1 an estate", "Gives %1").
					arg(p->name()));
	}
	
	new PlayerListViewItem(mPlayerList, p, p->name());
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





