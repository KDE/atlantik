#ifndef TRADEWIDGET_H
#define TRADEWIDGET_H

#include <qwidget.h>

class AtlanticCore;

class GivingListViewItem;
class PlayerListViewItem;
class TradeListViewItem;

class KListView;
class QListViewItem;
class Trade;
class Player;
class TradeItem;

class TradeDisplay : public QWidget
{
Q_OBJECT

public:
	TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent=0, const char *name = 0);

	Trade *trade() { return mTrade; }

	GivingListViewItem *giving(Player *from, Player *to);
	GivingListViewItem *giving(TradeItem *t);
	PlayerListViewItem *player(Player *from);
	TradeListViewItem  *trade(TradeItem *t);
	
private slots:
	void playerAdded(Player *);
	void playerRemoved(Player *);

	void tradeAdded(TradeItem *);
	void tradeRemoved(TradeItem *);
	void tradeChanged(TradeItem *);

	void contextMenu(KListView *l, QListViewItem *i);
	
private:
	AtlanticCore *m_atlanticCore;

	KListView *mPlayerList;
	Trade *mTrade;
};

#endif

