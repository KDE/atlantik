#include <kdebug.h>

#include "trade.moc"
#include "player.h"

Trade::Trade(int tradeId) : QObject()
{
	m_tradeId = tradeId;
	m_changed = false;
}

void Trade::addPlayer(Player *player)
{
	kdDebug() << "Trade::addPlayer" << endl;
	playerMap[player->playerId()] = player;
	m_changed = true;
	update();
}

void Trade::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}
