#include <iostream>

#include "trade.h"
#include "trade.moc"
#include "player.h"
#include "estate.h"

Trade::Trade(int tradeId)
{
	m_tradeId = tradeId;

	m_changed = false;
}

void Trade::addPlayer(Player *player)
{
	mPlayers.append(player);
	m_playerAcceptMap[player] = false;
}

QPtrList<Player> Trade::players() const
{
	return mPlayers;
}

unsigned int Trade::acceptCount()
{
	unsigned int count = 0;
	for (QMapIterator<Player *, bool> it = m_playerAcceptMap.begin() ; it != m_playerAcceptMap.end() ; ++it)
	{
		if (it.data())
			count++;
	}
	return count;
}

void Trade::updateEstate(Estate *estate, Player *to)
{
	std::cout << "Trade::updateEstate" << endl;	
	TradeEstate *t=0;
	
	for (QPtrListIterator<TradeItem> i(mTradeItems); *i; ++i)
	{
		t=dynamic_cast<TradeEstate*>(*i);

		if (!t)
			continue;
		
		if (t->estate()==estate)
			break;
		
		t=0;
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
			delete t;
			mTradeItems.removeRef(t);
			emit itemRemoved(t);
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
	std::cout << "Trade::updateMoney" << endl;	
	TradeMoney *t=0;
	
	for (QPtrListIterator<TradeItem> i(mTradeItems); *i; ++i)
	{
		t=dynamic_cast<TradeMoney*>(*i);

		if (!t)
			continue;
		
		if (t->from() == from && t->to() == to && t->money())
			break;
		
		t=0;
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
			delete t;
			mTradeItems.removeRef(t);
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

void Trade::updateAccept(Player *player, bool accept)
{
	if (m_playerAcceptMap[player] != accept)
	{
		m_playerAcceptMap[player] = accept;
		m_changed = true;
	}
}

void Trade::reject(Player *player)
{
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
	connect(from, SIGNAL(changed(Player *)), this, SIGNAL(changed(TradeItem *)));
	connect(to, SIGNAL(changed(Player *)), this, SIGNAL(changed(TradeItem *)));
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

TradeMoney::TradeMoney(unsigned int money, Trade *trade, Player *from, Player *to) : TradeItem(trade, from, to), mMoney(money)
{
}

QString TradeMoney::text() const
{
	return QString("$%1").arg(mMoney);
}
