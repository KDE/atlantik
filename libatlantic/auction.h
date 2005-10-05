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

#ifndef LIBATLANTIC_AUCTION_H
#define LIBATLANTIC_AUCTION_H

#include <qobject.h>

#include "libatlantic_export.h"

class Player;
class Estate;

class LIBATLANTIC_EXPORT Auction : public QObject
{
Q_OBJECT

public:
	Auction(int auctionId, Estate *estate);
	virtual ~Auction();

	int auctionId() { return m_auctionId; }
	Estate *estate() { return m_estate; }

	void setStatus(int status);
	int status() { return m_status; }

	void newBid(Player *player, int bid);

	void update(bool force = false);

signals:
	void changed();
	void completed();
	void bid(Auction *auction, int amount);
	void updateBid(Player *player, int amount);

private:
	bool m_changed;
	int m_auctionId, m_status;
	Estate *m_estate;
};

#endif
