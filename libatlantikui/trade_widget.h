#ifndef TRADEWIDGET_H
#define TRADEWIDGET_H

#include <qwidget.h>
#include <qmap.h>

class QListViewItem;
class QLabel;
class QSpinBox;

class KListView;
class KComboBox;
class KPushButton;

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

protected:
	void closeEvent(QCloseEvent *e);

private slots:
	void tradeItemAdded(TradeItem *);
	void tradeItemRemoved(TradeItem *);
	void tradeItemChanged(TradeItem *);
	void tradeChanged();
	void tradeRejected(Player *);

	void setEditType(int);
	void setEditEstate(int);

	void updateComponent();
	void reject();
	void accept();

	void contextMenu(KListView *l, QListViewItem *i, const QPoint& p);
	void contextMenuClicked(int item);

signals:
	void updateEstate(Trade *trade, Estate *estate, Player *to);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);
	void reject(Trade *trade);
	void accept(Trade *trade);

private:
	QLabel *m_status, *m_fromLabel, *m_toLabel;
	QSpinBox *m_moneyBox;

	KComboBox *m_editTypeCombo, *m_playerFromCombo, *m_playerTargetCombo, *m_estateCombo;
	KListView *m_componentList;
	KPushButton *m_updateButton, *m_rejectButton, *m_acceptButton;

	AtlanticCore *m_atlanticCore;
	Trade *mTrade, *m_trade;
	TradeItem *m_contextTradeItem;

	QMap<TradeItem *, KListViewItem *> m_componentMap;
	QMap<KListViewItem *, TradeItem *> m_componentRevMap;
	QMap<int, Estate *> m_estateMap;
	QMap<int, Player *> m_playerFromMap, m_playerTargetMap;
	QMap<Player *, int> m_playerFromRevMap, m_playerTargetRevMap;
};

#endif
