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
#include <QString>
#include <q3ptrlist.h>
#include <QMap>
#include "libatlantic_export.h"

class Player;
class Trade;
class Estate;

class LIBATLANTIC_EXPORT TradeItem : public QObject
{
Q_OBJECT

public:
	TradeItem(Trade *trade, Player *from, Player *to);
	virtual ~TradeItem() { }
	
	Player *from() { return mFrom; }
	Player *to() { return mTo; }
	void setTo(Player *p) { mTo=p; }
	Trade *trade() { return mTrade; }

	/**
	 * how to visualize this
	 **/
	virtual QString text() const=0;

signals:
	void changed(TradeItem *);

private slots:
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

	Estate *estate() { return mEstate; }
	
	virtual QString text() const;

signals:
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
	
	virtual QString text() const;

signals:
	void changed(TradeItem *tradeItem);

private:
	unsigned int m_money;
};


class LIBATLANTIC_EXPORT Trade : public QObject
{
Q_OBJECT

public:
	Trade(int tradeId);
	int tradeId() { return m_tradeId; }

	void setRevision(int revision);
	int revision() const;

	void addPlayer(Player *player);
	void removePlayer(Player *player);
	
	unsigned int count( bool acceptOnly );

	bool isRejected() { return m_rejected; }
	
#if 0
	/**
	 * select the Trade of the given template type, with the given
	 * from and to
	 **/
	template <class Type> Type *select(Player *from, Player *to)
	{
		for (Q3PtrListIterator<Player> i(mPlayers); *i; ++i)
		{
			if (
					i.current()->from()==from
					&& i.current()->to()==to
					&& dynamic_cast<Player*>(*i)
				)
			{
				return *i;
			}
		}
	
	}
#endif

private slots:	
	/**
	 * tell someone that this changed
	 **/
//	void changed(TradeItem *i) { emit itemChanged(i); }

public:
	void update(bool force = false);
	void updateEstate(Estate *estate, Player *player);
	void updateMoney(unsigned int money, Player *from, Player *to);
	void updateAccept(Player *player, bool accept);
	void reject(Player *player);
	
signals:
	void changed(Trade *);
	void rejected(Player *player);

	void itemAdded(TradeItem *);
	void itemRemoved(TradeItem *);

	void updateEstate(Trade *trade, Estate *estate, Player *to);
	void updateMoney(Trade *trade, unsigned int money, Player *from, Player *to);
	void reject(Trade *trade);
	void accept(Trade *trade);

private:
	bool m_changed, m_rejected;
	int m_tradeId, m_revision;

	Q3PtrList<Player> mPlayers;
	QMap<Player *, bool> m_playerAcceptMap;
	
	Q3PtrList<TradeItem> mTradeItems;
};

#endif
