// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

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
