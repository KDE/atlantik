#include <kdebug.h>
#include "network.h"
#include "trade.moc"
#include "player.h"
#include "trade_widget.h"
#include "estate.h"

Trade::Trade(GameNetwork *network, int tradeId)
{
	m_tradeId = tradeId;

	mDisplay=new TradeDisplay(this);
	connect(this, SIGNAL(changed()), mDisplay, SLOT(tradeChanged()));
	
}

void Trade::addPlayer(Player *player)
{
	kdDebug() << "Trade::addPlayer" << endl;
	mPlayers.append(player);
	emit playerAdded(player);
}

QPtrList<Player> Trade::players() const
{
	return mPlayers;
}

void Trade::addTradeItem(TradeItem *i)
{
	mTradeItems.append(i);
	i->createCommand();

}

void Trade::updateEstate(Estate *estate, Player *player)
{
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
			if (t->to()==player) return;
			t->setTo(player);
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
		emit tradeAdded(t);
	}
}

void Trade::updateMoney(Player *from, Player *to, unsigned int money)
{

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

}

void TradeEstate::destroyCommand()
{

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



