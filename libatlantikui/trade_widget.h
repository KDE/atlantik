#ifndef TRADEWIDGET_H
#define TRADEWIDGET_H

#include <qwidget.h>
#include <qmap.h>

class QListViewItem;

class KListView;

class AtlanticCore;
class Player;
class Trade;
class TradeItem;

class TradeDisplay : public QWidget
{
Q_OBJECT

public:
	TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent=0, const char *name = 0);

	Trade *trade() { return mTrade; }

private slots:
	void playerAdded(Player *);
	void playerRemoved(Player *);

	void tradeAdded(TradeItem *);
	void tradeRemoved(TradeItem *);
	void tradeChanged(TradeItem *);

	void contextMenu(KListView *l, QListViewItem *i);
	
private:
	AtlanticCore *m_atlanticCore;
	KListView *m_componentList;
	Trade *mTrade, *m_trade;

	QMap<TradeItem *, KListViewItem *> m_componentMap;
};

#endif
