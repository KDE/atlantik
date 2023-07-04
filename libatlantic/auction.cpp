// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "auction.h"
#include "player.h"
#include "estate.h"

Auction::Auction(int auctionId, Estate *estate)
	: QObject()
	, m_changed(false)
	, m_auctionId(auctionId)
	, m_status(0)
	, m_estate(estate)
{
}

Auction::~Auction()
{
	Q_EMIT completed();
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
	Q_EMIT updateBid(player, amount);
}

void Auction::update(bool force)
{
	if (m_changed || force)
	{
		Q_EMIT changed();
		m_changed = false;
	}
}

#include "moc_auction.cpp"
