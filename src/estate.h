#ifndef ATLANTIK_ESTATE_H
#define ATLANTIK_ESTATE_H

#include "estateview.h"

class Player;

class Estate
{
public:
	Estate(int estateId);
	const int estateId() { return m_estateId; }
	void setView(EstateView *estateView) { m_estateView = estateView; }
	void setName(const QString name);
	const QString name() { return m_name; }
	void setOwner(Player *player);
	void setCanBeOwned(const bool canBeOwned);
	const bool canBeOwned() { return m_canBeOwned; }

private:
	int m_estateId;
	EstateView *m_estateView;
	QString m_name;
	Player *m_owner;
	bool m_canBeOwned, m_canBeMortgaged, m_canBeUnmortgaged, m_mortgaged;
};

#endif
