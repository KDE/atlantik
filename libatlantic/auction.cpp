#include "auction.h"
#include "auction.moc"

Auction::Auction(int auctionId) : QObject()
{
	m_auctionId = auctionId;
	m_changed = false;
}

void Auction::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}
