#ifndef ATLANTIK_TRADE_H
#define ATLANTIK_TRADE_H

#include <qstring.h>
#include <qmap.h>

#include <portfolioview.h>

//#include "player.h"

class Player;

class Trade : public QObject
{
Q_OBJECT

public:
	Trade(int tradeId);
	int tradeId() { return m_tradeId; }

	void addPlayer(Player *);
	
	void update(bool force = false);

signals:
	void changed();

private:
	bool m_changed;
	int m_tradeId;

	QMap<int, Player *> playerMap;
};

#endif
