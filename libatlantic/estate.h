#ifndef LIBATLANTIC_ESTATE_H
#define LIBATLANTIC_ESTATE_H

#include <qobject.h>
#include <qcolor.h>

class Player;

class Estate : public QObject
{
Q_OBJECT

public:
	Estate(int estateId);
	int estateId() const { return m_estateId; }
	void setName(const QString name);
	QString name() const { return m_name; }
	void setGroup(const QString group);
	QString group() const { return m_group; }
	void setOwner(Player *player);
	bool isOwned() const;
	bool isOwnedBySelf() const;
	Player *owner() { return m_owner; }
	void setHouses(unsigned int houses);
	unsigned int houses() { return m_houses; }
	void setCanBeOwned(const bool canBeOwned);
	bool canBeOwned() const { return m_canBeOwned; }
	void setCanBuyHouses(const bool canBuyHouses);
	bool canBuyHouses() const { return m_canBuyHouses; }
	void setCanSellHouses(const bool canSellHouses);
	bool canSellHouses() const { return m_canSellHouses; }
	void setIsMortgaged(const bool isMortgaged);
	bool isMortgaged() const { return m_isMortgaged; }
	void setCanToggleMortgage(const bool canToggleMortgage);
	bool canToggleMortgage() const { return m_canToggleMortgage; }
	void setColor(const QColor color);
	QColor color() const { return m_color; }
	void setBgColor(const QColor color);
	QColor bgColor() const { return m_bgColor; }
	void update(bool force = false);

signals:
	void changed();
	void estateToggleMortgage(Estate *estate);
	void estateHouseBuy(Estate *estate);
	void estateHouseSell(Estate *estate);
	void newTrade(Player *player);
	void LMBClicked(Estate *estate);

protected:
	bool m_changed;
	int m_estateId;

private:
	QString m_name, m_group;
	Player *m_owner;
	unsigned int m_houses;
	bool m_canBeOwned, m_canBuyHouses, m_canSellHouses, m_isMortgaged, m_canToggleMortgage;
	QColor m_bgColor, m_color;
};

#endif
