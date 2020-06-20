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

#include <QDialog>
#include <QMap>

#include "libatlantikui_export.h"

class QComboBox;
class QLabel;
class QGroupBox;
class QPushButton;
class QSpinBox;
class QTreeWidget;
class QTreeWidgetItem;

class AtlanticCore;
class Card;
class Estate;
class Player;
class Trade;
class TradeItem;

class LIBATLANTIKUI_EXPORT TradeDisplay : public QDialog
{
Q_OBJECT

public:
	TradeDisplay(Trade *trade, AtlanticCore *atlanticCore, QWidget *parent=nullptr);

	Trade *trade() const { return m_trade; }

protected:
	void closeEvent(QCloseEvent *e) override;

private Q_SLOTS:
	void tradeItemAdded(TradeItem *);
	void tradeItemRemoved(TradeItem *);
	void tradeItemChanged(TradeItem *);
	void tradeChanged();
	void playerChanged(Player *player);
	void tradeRejected(Player *);
	void slotPlayerAdded(Player *player);
	void slotPlayerRemoved(Player *player);
	void slotAcceptChanged(Player *player, bool accept);

	void setTypeCombo(int);
	void setEstateCombo(int);
	void setCardCombo(int);
	void setCombos(QTreeWidgetItem *i);

	void updateComponent();
	void reject() override;
	void accept() override;

	void contextMenu(const QPoint& pos);
	void contextMenuClickedRemove();

Q_SIGNALS:
	void updateEstate(Trade *trade, Estate *estate, Player *to);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);
	void updateCard(Trade *trade, Card *card, Player *to);
	void reject(Trade *trade);
	void accept(Trade *trade);

private:
	QGroupBox *m_updateComponentBox;
	QLabel *m_status, *m_fromLabel, *m_toLabel;
	QSpinBox *m_moneyBox;

	QComboBox *m_editTypeCombo, *m_playerFromCombo, *m_playerTargetCombo, *m_estateCombo, *m_cardCombo;
	QTreeWidget *m_componentList;
	QPushButton *m_updateButton, *m_rejectButton, *m_acceptButton;
	QTreeWidget *m_participantsList;

	AtlanticCore *m_atlanticCore;
	Trade *m_trade;
	TradeItem *m_contextTradeItem;

	// TODO: Wouldn't QPair make more sense here?
	QMap<TradeItem *, QTreeWidgetItem *> m_componentMap;
	QMap<QTreeWidgetItem *, TradeItem *> m_componentRevMap;
	QMap<int, Estate *> m_estateMap;
	QMap<Estate *, int> m_estateRevMap;
	QMap<int, Player *> m_playerFromMap, m_playerTargetMap;
	QMap<Player *, int> m_playerFromRevMap, m_playerTargetRevMap;
	QMap<int, Card *> m_cardMap;
	QMap<Card *, int> m_cardRevMap;
	QMap<Player *, QTreeWidgetItem *> m_playerListMap;
};

#endif
