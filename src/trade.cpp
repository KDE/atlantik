#include <kdebug.h>

#include "trade.moc"

Trade::Trade(int tradeId) : QObject()
{
	m_tradeId = tradeId;
	m_changed = false;
}

void Trade::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}
