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

#include "trade.h"
#include "player.h"
#include "estate.h"
#include "card.h"

Trade::Trade(int tradeId)
	: QObject()
	, m_changed(false)
	, m_rejected(false)
	, m_tradeId(tradeId)
	, m_revision(0)
{
}

void Trade::setRevision(int revision)
{
	m_revision = revision;
}

int Trade::revision() const
{
	return m_revision;
}

void Trade::addPlayer(Player *player)
{
	m_playerAcceptMap[player] = false;
	emit playerAdded(player);
}

void Trade::removePlayer(Player *player)
{
	m_playerAcceptMap[player] = false;
	emit playerRemoved(player);
}

unsigned int Trade::count( bool acceptOnly ) const
{
	unsigned int count = 0;
	QMapIterator<Player *, bool> it(m_playerAcceptMap);
	while(it.hasNext())
	{
		it.next();
		if ( !acceptOnly || it.value() )
			count++;
	}
	return count;
}

QList<Player *> Trade::participants() const
{
	return m_playerAcceptMap.keys();
}

void Trade::updateEstate(Estate *estate, Player *to)
{
	TradeEstate *t=Q_NULLPTR;
	
	foreach (TradeItem *i, mTradeItems)
	{
		t=dynamic_cast<TradeEstate*>(i);

		if (!t)
			continue;
		
		if (t->estate()==estate)
			break;
		
		t=Q_NULLPTR;
	}
	if (t)
	{
		if (to)
		{
			if (t->to() == to)
				return;
			t->setTo(to);
		}
		else
		{
			mTradeItems.removeOne(t);
			emit itemRemoved(t);
			t->deleteLater();
		}
	}
	else if (estate && to)
	{
		// new trade
		t = new TradeEstate(estate, this, to);
		
		mTradeItems.append(t);
		emit itemAdded(t);
	}
}

void Trade::updateMoney(unsigned int money, Player *from, Player *to)
{
	TradeMoney *t=Q_NULLPTR;
	
	foreach (TradeItem *i, mTradeItems)
	{
		t=dynamic_cast<TradeMoney*>(i);

		if (!t)
			continue;
		
		if (t->from() == from && t->to() == to && t->money())
			break;
		
		t=Q_NULLPTR;
	}
	if (t)
	{
		if (from && to && money)
		{
			if (t->money() == money)
				return;
			t->setMoney(money);
		}
		else
		{
			mTradeItems.removeOne(t);
			emit itemRemoved(t);
			t->deleteLater();
		}
	}
	else if (from && to && money)
	{
		// new trade
		t = new TradeMoney(money, this, from, to);
		
		mTradeItems.append(t);
		emit itemAdded(t);
	}
}

void Trade::updateCard(Card *card, Player *to)
{
	TradeCard *t=Q_NULLPTR;
	
	foreach (TradeItem *i, mTradeItems)
	{
		t=dynamic_cast<TradeCard*>(i);

		if (!t)
			continue;
		
		if (t->card()==card)
			break;
		
		t=Q_NULLPTR;
	}
	if (t)
	{
		if (to)
		{
			if (t->to() == to)
				return;
			t->setTo(to);
		}
		else
		{
			mTradeItems.removeOne(t);
			emit itemRemoved(t);
			t->deleteLater();
		}
	}
	else if (card && to)
	{
		// new trade
		t = new TradeCard(card, this, to);
		
		mTradeItems.append(t);
		emit itemAdded(t);
	}
}

void Trade::updateAccept(Player *player, bool accept)
{
	const bool hadItem = m_playerAcceptMap.contains(player);
	if (m_playerAcceptMap[player] != accept)
	{
		m_playerAcceptMap[player] = accept;
		emit acceptChanged(player, accept);
		m_changed = true;
	} else if (!hadItem)
		emit acceptChanged(player, accept);
}

void Trade::reject(Player *player)
{
	m_rejected = true;
	emit rejected(player);
}

void Trade::update(bool force)
{
	if (m_changed || force)
	{
		emit changed(this);
		m_changed = false;
	}
}

TradeItem::TradeItem(Trade *trade, Player *from, Player *to) : mFrom(from), mTo(to), mTrade(trade)
{
	connect(from, SIGNAL(changed(Player *)), this, SLOT(playerChanged()));
	connect(to, SIGNAL(changed(Player *)), this, SLOT(playerChanged()));
}

void TradeItem::playerChanged()
{
	emit changed(this);
}

TradeEstate::TradeEstate(Estate *estate, Trade *trade, Player *to) : TradeItem(trade, estate->owner(), to), mEstate(estate)
{
}

QString TradeEstate::text() const
{
	return mEstate->name();
}

TradeMoney::TradeMoney(unsigned int money, Trade *trade, Player *from, Player *to) : TradeItem(trade, from, to), m_money(money)
{
}

void TradeMoney::setMoney(unsigned int money)
{
	if (m_money != money)
	{
		m_money = money;
		emit changed(this);
	}
}

QString TradeMoney::text() const
{
	return QStringLiteral("$%1").arg(m_money);
}

TradeCard::TradeCard(Card *card, Trade *trade, Player *to) : TradeItem(trade, card->owner(), to), mCard(card)
{
}

QString TradeCard::text() const
{
	return mCard->title();
}
