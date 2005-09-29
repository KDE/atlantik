// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef LIBATLANTIC_ESTATE_H
#define LIBATLANTIC_ESTATE_H

#include <qobject.h>
#include <qcolor.h>

#include "libatlantic_export.h"

class EstateGroup;
class Player;

class LIBATLANTIC_EXPORT Estate : public QObject
{
Q_OBJECT

public:
	Estate(int estateId);
	int id() { return m_id; }
	void setName(QString name);
	QString name() const;
	void setEstateGroup(EstateGroup *estateGroup);
	EstateGroup *estateGroup() { return m_estateGroup; }
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
	void setHousePrice(const unsigned int housePrice) { m_housePrice = housePrice; }
        unsigned int housePrice() const { return m_housePrice; }
        void setHouseSellPrice(const unsigned int houseSellPrice) { m_houseSellPrice = houseSellPrice; }
        unsigned int houseSellPrice() const { return m_houseSellPrice; }
	void setIsMortgaged(const bool isMortgaged);
	bool isMortgaged() const { return m_isMortgaged; }
	void setCanToggleMortgage(const bool canToggleMortgage);
	bool canToggleMortgage() const { return m_canToggleMortgage; }
	void setMortgagePrice(const unsigned int mortgagePrice) { m_mortgagePrice = mortgagePrice; }
        unsigned int mortgagePrice() const { return m_mortgagePrice; }
        void setUnmortgagePrice(const unsigned int unmortgagePrice) { m_unmortgagePrice = unmortgagePrice; }
        unsigned int unmortgagePrice() const { return m_unmortgagePrice; }
	void setColor(QColor color);
	QColor color() const { return m_color; }
	void setBgColor(QColor color);
	QColor bgColor() const { return m_bgColor; }
	void setPrice(const unsigned int price) { m_price = price; }
	unsigned int price() const { return m_price; }
	void setMoney(int money);
	int money();
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
	int m_id;

private:
	QString m_name;
	Player *m_owner;
	EstateGroup *m_estateGroup;
	unsigned int m_houses, m_price, m_housePrice, m_houseSellPrice, m_mortgagePrice, m_unmortgagePrice;
	int m_money;
	bool m_canBeOwned, m_canBuyHouses, m_canSellHouses, m_isMortgaged, m_canToggleMortgage;
	QColor m_bgColor, m_color;
};

#endif
