#include <iostream>

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
	emit playerAdded(player);
}

QPtrList<Player> Trade::players() const
{
	return mPlayers;
}

void Trade::removeTradeItem(TradeItem *i)
{
	mTradeItems.removeRef(i);
	i->destroyCommand();
}

void Trade::addTradeItem(TradeItem *i)
{
	mTradeItems.append(i);
	i->createCommand();
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
			std::cout << "gonna emit tradeChanged" << endl;
			t->setTo(to);
			std::cout << "emit tradeChanged" << endl;
			emit tradeChanged(t);
		}
		else
		{
			delete t;
			mTradeItems.removeRef(t);
			emit tradeRemoved(t);
		}
	}
	else if (estate && to)
	{
		// new trade
		t = new TradeEstate(estate, this, to);
		
		mTradeItems.append(t);
			std::cout << "emit tradeAdded" << endl;
		emit tradeAdded(t);
	}
}

void Trade::updateMoney(Player *from, Player *to, unsigned int money)
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
			std::cout << "gonna emit tradeChanged" << endl;
			t->setMoney(money);
			std::cout << "emit tradeChanged" << endl;
			emit tradeChanged(t);
		}
		else
		{
			delete t;
			mTradeItems.removeRef(t);
			emit tradeRemoved(t);
		}
	}
	else if (from && to && money)
	{
		// new trade
		t = new TradeMoney(money, this, from, to);
		
		mTradeItems.append(t);
			std::cout << "emit tradeAdded" << endl;
		emit tradeAdded(t);
	}
}

void Trade::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}

TradeItem::TradeItem(Trade *trade, Player *from, Player *to)
	: mFrom(from), mTo(to), mTrade(trade)
{

}

TradeEstate::TradeEstate(Estate *estate, Trade *trade, Player *to)
	: TradeItem(trade, estate->owner(), to), mEstate(estate)
{

}

QString TradeEstate::text() const
{
	return mEstate->name();
}

void TradeEstate::createCommand()
{
	emit updateEstate(trade(), estate(), to());
}

void TradeEstate::destroyCommand()
{
	emit updateEstate(trade(), estate(), to());
}


TradeMoney::TradeMoney(unsigned int money, Trade *trade, Player *from, Player *to)
	: TradeItem(trade, from, to), mMoney(money)
{

}

QString TradeMoney::text() const
{
	return QString("$%1").arg(mMoney);
}

void TradeMoney::createCommand()
{

}

void TradeMoney::destroyCommand()
{

}
