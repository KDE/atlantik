#ifndef ATLANTIK_ESTATE_H
#define ATLANTIK_ESTATE_H

#include "estateview.h"

class Player;

class Estate : public QObject
{
Q_OBJECT

public:
	Estate(int estateId);
	const int estateId() { return m_estateId; }
	void setGroupId(const int groupId);
	const int groupId() { return m_groupId; }
	void setName(const QString name);
	const QString name() { return m_name; }
	void setOwner(Player *player);
	const bool isOwned();
	const bool isOwnedBySelf();
	Player *owner() { return m_owner; }
	void setHouses(unsigned int houses);
	unsigned int houses() { return m_houses; }
	void setCanBeOwned(const bool canBeOwned);
	const bool canBeOwned() { return m_canBeOwned; }
	void setCanBuyHouses(const bool canBuyHouses);
	const bool canBuyHouses() { return m_canBuyHouses; }
	void setCanSellHouses(const bool canSellHouses);
	const bool canSellHouses() { return m_canSellHouses; }
	void setIsMortgaged(const bool isMortgaged);
	const bool isMortgaged() { return m_isMortgaged; }
	void setCanToggleMortgage(const bool canToggleMortgage);
	const bool canToggleMortgage() { return m_canToggleMortgage; }
	void setColor(const QColor color);
	const QColor color() { return m_color; }
	void setBgColor(const QColor color);
	const QColor bgColor() { return m_bgColor; }
	void update(bool force = false);

signals:
	void changed();
	void estateToggleMortgage(int estateId);
	void estateHouseBuy(int estateId);
	void estateHouseSell(int estateId);

private:
	bool m_changed;
	int m_estateId, m_groupId;
	QString m_name;
	Player *m_owner;
	unsigned int m_houses;
	bool m_canBeOwned, m_canBuyHouses, m_canSellHouses, m_isMortgaged, m_canToggleMortgage;
	QColor m_bgColor, m_color;
};

#endif
