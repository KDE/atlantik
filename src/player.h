#ifndef ATLANTIK_PLAYER_H
#define ATLANTIK_PLAYER_H

#include <qstring.h>

#include <portfolioview.h>

class Player
{
public:
	Player(uint id);
	void setName(const QString _n);
	void setMoney(const QString _m);
	void setView(PortfolioView *_pfv) { m_portfolioView = _pfv; }

private:
	uint m_id;
	QString m_name, m_money;
	PortfolioView *m_portfolioView;
};

#endif
