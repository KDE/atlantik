#include "atlantic_core.moc"

#include "player.h"
#include "estate.h"
#include "trade.h"

AtlanticCore::AtlanticCore(QObject *parent, const char *name) : QObject(parent, name)
{
}
QPtrList<Player> AtlanticCore::players()
{
	return m_players;
}

Player *AtlanticCore::newPlayer(int playerId)
{
	Player *player = new Player(playerId);
	m_players.append(player);
	return player;
}

QPtrList<Estate> AtlanticCore::estates()
{
	return m_estates;
}

Estate *AtlanticCore::newEstate(int estateId)
{
	Estate *estate = new Estate(estateId);
	m_estates.append(estate);
	return estate;
}

QPtrList<Trade> AtlanticCore::trades()
{
	return m_trades;
}

Trade *AtlanticCore::newTrade(int tradeId)
{
	Trade *trade = new Trade(tradeId);
	m_trades.append(trade);
	return trade;
}

void AtlanticCore::setCurrentTurn(Player *player)
{
	Player *p;
	for (QPtrListIterator<Player> i(m_players); *i; ++i)
	{
		p = dynamic_cast<Player *>(*i);
		if (p)
			p->setHasTurn(p==player);
	}
}
