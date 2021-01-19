// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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

#ifndef LIBATLANTIC_TRADE_H
#define LIBATLANTIC_TRADE_H

#include <QObject>
#include <QMap>
#include <QList>

#include "libatlantic_export.h"

class Player;
class Trade;
class Estate;
class Card;

class LIBATLANTIC_EXPORT TradeItem : public QObject
{
Q_OBJECT

public:
	TradeItem(Trade *trade, Player *from, Player *to);
	virtual ~TradeItem() { }
	
	Player *from() const { return mFrom; }
	Player *to() const { return mTo; }
	void setTo(Player *p) { mTo=p; }
	Trade *trade() const { return mTrade; }

	/**
	 * how to visualize this
	 **/
	virtual QString text() const=0;

Q_SIGNALS:
	void changed(TradeItem *);

private Q_SLOTS:
	void playerChanged();

private:
	Player *mFrom, *mTo;
	Trade *mTrade;
};

class LIBATLANTIC_EXPORT TradeEstate : public TradeItem
{
Q_OBJECT

public:
	TradeEstate(Estate *estate, Trade *trade, Player *to);

	Estate *estate() const { return mEstate; }
	
	QString text() const override;

Q_SIGNALS:
	void updateEstate(Trade *trade, Estate *estate, Player *player);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);

private:
	Estate *mEstate;
};

class LIBATLANTIC_EXPORT TradeMoney : public TradeItem
{
Q_OBJECT

public:
	TradeMoney(unsigned int money, Trade *trade, Player *from, Player *to);

	unsigned int money() const { return m_money; }
	void setMoney(unsigned int money);
	
	QString text() const override;

Q_SIGNALS:
	void changed(TradeItem *tradeItem);

private:
	unsigned int m_money;
};

class LIBATLANTIC_EXPORT TradeCard : public TradeItem
{
Q_OBJECT

public:
	TradeCard(Card *card, Trade *trade, Player *to);

	Card *card() const { return mCard; }
	
	QString text() const override;

Q_SIGNALS:

private:
	Card *mCard;
};


class LIBATLANTIC_EXPORT Trade : public QObject
{
Q_OBJECT

public:
	Trade(int tradeId);
	int tradeId() const { return m_tradeId; }

	void setRevision(int revision);
	int revision() const;

	void addPlayer(Player *player);
	void removePlayer(Player *player);
	
	unsigned int count( bool acceptOnly ) const;

	bool isRejected() const { return m_rejected; }

	QList<Player *> participants() const;

private Q_SLOTS:
	/**
	 * tell someone that this changed
	 **/
//	void changed(TradeItem *i) { Q_EMIT itemChanged(i); }

public:
	void update(bool force = false);
	void updateEstate(Estate *estate, Player *player);
	void updateMoney(unsigned int money, Player *from, Player *to);
	void updateCard(Card *card, Player *to);
	void updateAccept(Player *player, bool accept);
	void reject(Player *player);
	
Q_SIGNALS:
	void changed(Trade *);
	void rejected(Player *player);

	void itemAdded(TradeItem *);
	void itemRemoved(TradeItem *);

	void updateEstate(Trade *trade, Estate *estate, Player *to);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);
	void updateCard(Trade *trade, Card *card, Player *to);
	void acceptChanged(Player *player, bool accept);
	void reject(Trade *trade);
	void accept(Trade *trade);

	void playerAdded(Player *player);
	void playerRemoved(Player *player);

private:
	bool m_changed, m_rejected;
	int m_tradeId, m_revision;

	QMap<Player *, bool> m_playerAcceptMap;
	
	QList<TradeItem *> mTradeItems;
};

#endif
