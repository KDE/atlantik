#ifndef LIBATLANTIC_CORE_H
#define LIBATLANTIC_CORE_H

#include <qobject.h>
#include <qptrlist.h>

class Player;
class Estate;
class EstateGroup;
class Trade;
class Auction;

class AtlanticCore : public QObject
{
Q_OBJECT

public:
	AtlanticCore(QObject *parent, const char *name);

	QPtrList<Player> players();
	Player *newPlayer(int playerId);

	QPtrList<Estate> estates();
	Estate *newEstate(int estateId);

	QPtrList<EstateGroup> estateGroups();
	EstateGroup *newEstateGroup(QString name);

	QPtrList<Trade> trades();
	Trade *newTrade(int tradeId);
	void delTrade(Trade *trade);

	QPtrList<Auction> auctions();
	Auction *newAuction(int auctionId, Estate *estate);
	void delAuction(Auction *auction);

	void setCurrentTurn(Player *player);

signals:
	void removeGUI(Trade *trade);

private:
	QPtrList<Player> m_players;
	QPtrList<Estate> m_estates;
	QPtrList<EstateGroup> m_estateGroups;
	QPtrList<Trade> m_trades;
	QPtrList<Auction> m_auctions;
};

#endif
