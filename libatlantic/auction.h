#ifndef LIBATLANTIC_AUCTION_H
#define LIBATLANTIC_AUCTION_H

#include <qobject.h>

class Auction : public QObject
{
Q_OBJECT

public:
	Auction(int auctionId);
	int auctionId() { return m_auctionId; }

	void update(bool force = false);

signals:
	void changed();

private:
	bool m_changed;
	int m_auctionId;
};

#endif
