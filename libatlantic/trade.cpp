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

void Trade::updateEstate(Estate *estate, Player *player)
{
	std::cout << "Trade::updateEstate" << endl;	
	TradeEstate *t=0;
	
	for (QPtrListIterator<TradeItem> i(mTradeItems); *i; ++i)
	{
		t=dynamic_cast<TradeEstate*>(*i);
		if (!t) continue;
		
		if (t->estate()==estate) break;
		
		t=0;
	}
	if (t)
	{
		if (player)
		{
			std::cout << "gonna emit tradeChanged" << endl;
			if (t->to()==player) return;
			t->setTo(player);
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
	else if (player)
	{
		// new trade
		t=new TradeEstate(estate, this, player);
		
		mTradeItems.append(t);
			std::cout << "emit tradeAdded" << endl;
		emit tradeAdded(t);
	}
}

void Trade::updateMoney(Player *from, Player *to, unsigned int money)
{

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
	emit tradeUpdateEstate(trade(), estate(), to());
}

void TradeEstate::destroyCommand()
{
	emit tradeUpdateEstate(trade(), estate(), to());
}


TradeCash::TradeCash(int cash, Trade *trade, Player *from, Player *to)
	: TradeItem(trade, from, to), mCash(cash)
{

}

QString TradeCash::text() const
{
	return QString("$%1").arg(cash());
}

void TradeCash::createCommand()
{

}

void TradeCash::destroyCommand()
{

}
