#ifndef LIBATLANTIC_AUCTION_H
#define LIBATLANTIC_AUCTION_H

#include <qobject.h>

class Player;
class Estate;

class Auction : public QObject
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