#ifndef LIBATLANTIC_CORE_H
#define LIBATLANTIC_CORE_H

#include <qobject.h>
#include <qptrlist.h>

class Player;
class Estate;
class Trade;

class AtlanticCore : public QObject
{
Q_OBJECT

public:
	AtlanticCore(QObject *parent, const char *name);

	QPtrList<Player> players();
	Player *newPlayer(int playerId);

	QPtrList<Estate> estates();
	Estate *newEstate(int estateId);

	QPtrList<Trade> trades();
	Trade *newTrade(int tradeId);

private:
	QPtrList<Player> m_players;
	QPtrList<Estate> m_estates;
	QPtrList<Trade> m_trades;
};

#endif
