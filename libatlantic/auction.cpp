#include "auction.h"
#include "auction.moc"
#include "player.h"
#include "estate.h"

Auction::Auction(int auctionId, Estate *estate) : QObject()
{
	m_auctionId = auctionId;
	m_estate = estate;
	m_changed = false;
}

Auction::~Auction()
{
	emit completed();
}

void Auction::setStatus(int status)
{
	if (m_status != status)
	{
		m_status = status;
		m_changed = true;
	}
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
