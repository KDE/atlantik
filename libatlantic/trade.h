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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef LIBATLANTIC_TRADE_H
#define LIBATLANTIC_TRADE_H

#include <qobject.h>
#include <qstring.h>
#include <qptrlist.h>

class Player;
class Trade;
class Estate;

class TradeItem : public QObject
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

class TradeEstate : public TradeItem
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

class TradeMoney : public TradeItem
{
public:
	TradeMoney(unsigned int money, Trade *trade, Player *from, Player *to);

	unsigned int money() const { return mMoney; }
	void setMoney(unsigned int money) { mMoney = money; }
	
	virtual QString text() const;

private:
	unsigned int mMoney;
};


class Trade : public QObject
{
Q_OBJECT

public:
	Trade(int tradeId);
	int tradeId() { return m_tradeId; }

	void addPlayer(Player *);
	
	QPtrList<Player> players() const;
	unsigned int acceptCount();
	
	/**
	 * select the Trade of the given template type, with the given
	 * from and to
	 **/
	template <class Type> Type *select(Player *from, Player *to)
	{
		for (QPtrListIterator<Player> i(mPlayers()); *i; ++i)
		{
			if (
					(*i)->from()==from
					&& (*i)->to()==to
					&& dynamic_cast<Player*>(*i)
				)
			{
				return *i;
			}
		}
	
	}

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
	bool m_changed;
	int m_tradeId;

	QPtrList<Player> mPlayers;
	QMap<Player *, bool> m_playerAcceptMap;
	
	QPtrList<TradeItem> mTradeItems;
};

#endif
