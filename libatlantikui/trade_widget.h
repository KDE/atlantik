// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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

#ifndef TRADEWIDGET_H
#define TRADEWIDGET_H

#include <QtCore/QMap>

#include <kdialog.h>

#include "libatlantikui_export.h"

class QLabel;
class Q3ListViewItem;
class Q3GroupBox;
class QSpinBox;

class K3ListView;
class K3ListViewItem;
class KComboBox;
class KPushButton;

class AtlanticCore;
class Estate;
class Player;
class Trade;
class TradeItem;

class LIBATLANTIKUI_EXPORT TradeDisplay : public KDialog
{
Q_OBJECT

public:
	TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent=0);

	Trade *trade() { return mTrade; }

protected:
	void closeEvent(QCloseEvent *e);

private Q_SLOTS:
	void tradeItemAdded(TradeItem *);
	void tradeItemRemoved(TradeItem *);
	void tradeItemChanged(TradeItem *);
	void tradeChanged();
	void playerChanged(Player *player);
	void tradeRejected(Player *);

	void setTypeCombo(int);
	void setEstateCombo(int);
	void setCombos(Q3ListViewItem *i);

	void updateComponent();
	void reject();
	void accept();

	void contextMenu(K3ListView *l, Q3ListViewItem *i, const QPoint& p);
	void contextMenuClicked(int item);

Q_SIGNALS:
	void updateEstate(Trade *trade, Estate *estate, Player *to);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);
	void reject(Trade *trade);
	void accept(Trade *trade);

private:
	Q3GroupBox *m_updateComponentBox;
	QLabel *m_status, *m_fromLabel, *m_toLabel;
	QSpinBox *m_moneyBox;

	KComboBox *m_editTypeCombo, *m_playerFromCombo, *m_playerTargetCombo, *m_estateCombo;
	K3ListView *m_componentList;
	KPushButton *m_updateButton, *m_rejectButton, *m_acceptButton;

	AtlanticCore *m_atlanticCore;
	Trade *mTrade, *m_trade;
	TradeItem *m_contextTradeItem;

	// TODO: Wouldn't QPair make more sense here?
	QMap<TradeItem *, K3ListViewItem *> m_componentMap;
	QMap<K3ListViewItem *, TradeItem *> m_componentRevMap;
	QMap<int, Estate *> m_estateMap;
	QMap<Estate *, int> m_estateRevMap;
	QMap<int, Player *> m_playerFromMap, m_playerTargetMap;
	QMap<Player *, int> m_playerFromRevMap, m_playerTargetRevMap;
};

#endif
