#include "auction.h"
#include "auction.moc"
#include "player.h"

Auction::Auction(int auctionId) : QObject()
{
	m_auctionId = auctionId;
	m_changed = false;
}

Auction::~Auction()
{
	emit completed();
}

void Auction::newBid(Player *player, int amount)
{
	emit updateBid(player, amount);
}

void Auction::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}
