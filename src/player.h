#ifndef ATLANTIK_PLAYER_H
#define ATLANTIK_PLAYER_H

#include <qstring.h>

#include <portfolioview.h>

class Player
{
public:
	Player(int playerId);
	int playerId() { return m_playerId; }

	void setIsSelf(const bool isSelf) { m_isSelf = isSelf; }
	const bool isSelf() { return m_isSelf; }

	void setName(const QString _n);
	const QString name() { return m_name; }

	void setMoney(const QString _m);
	void setView(PortfolioView *_pfv) { m_portfolioView = _pfv; }

private:
	int m_playerId;
	bool m_isSelf;
	QString m_name, m_money;
	PortfolioView *m_portfolioView;
};

#endif
