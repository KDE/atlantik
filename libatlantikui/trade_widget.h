#ifndef TRADEWIDGET_H
#define TRADEWIDGET_H

#include <qwidget.h>
#include <qmap.h>

class QListViewItem;
class QLabel;

class KListView;
class KComboBox;

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

	void updateEstate();

	void contextMenu(KListView *l, QListViewItem *i);

signals:
	void updateEstate(Trade *trade, Estate *estate, Player *to);	

private:
	QLabel *m_status;

	KComboBox *m_playerCombo, *m_estateCombo;
	KListView *m_componentList;

	AtlanticCore *m_atlanticCore;
	Trade *mTrade, *m_trade;

	QMap<TradeItem *, KListViewItem *> m_componentMap;
	QMap<int, Estate *> m_estateMap;
	QMap<int, Player *> m_playerMap;
};

#endif
