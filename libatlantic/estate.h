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

#include <QObject>
#include <QColor>

#include "libatlantic_export.h"

class EstateGroup;
class Player;

class LIBATLANTIC_EXPORT Estate : public QObject
{
Q_OBJECT

public:
	Estate(int estateId);
	int id() const { return m_id; }
	void setName(const QString& name);
	QString name() const;
	void setEstateGroup(EstateGroup *estateGroup);
	EstateGroup *estateGroup() const { return m_estateGroup; }
	void setOwner(Player *player);
	bool isOwned() const;
	bool isOwnedBySelf() const;
	Player *owner() const { return m_owner; }
	void setHouses(unsigned int houses);
	unsigned int houses() const { return m_houses; }
	void setCanBeOwned(bool canBeOwned);
	bool canBeOwned() const { return m_canBeOwned; }
	void setCanBuyHouses(bool canBuyHouses);
	bool canBuyHouses() const { return m_canBuyHouses; }
	void setCanSellHouses(bool canSellHouses);
	bool canSellHouses() const { return m_canSellHouses; }
	void setHousePrice(unsigned int housePrice);
        unsigned int housePrice() const { return m_housePrice; }
	void setHouseSellPrice(unsigned int houseSellPrice);
        unsigned int houseSellPrice() const { return m_houseSellPrice; }
	void setIsMortgaged(bool isMortgaged);
	bool isMortgaged() const { return m_isMortgaged; }
	void setCanToggleMortgage(bool canToggleMortgage);
	bool canToggleMortgage() const { return m_canToggleMortgage; }
	void setMortgagePrice(unsigned int mortgagePrice);
        unsigned int mortgagePrice() const { return m_mortgagePrice; }
	void setUnmortgagePrice(unsigned int unmortgagePrice);
        unsigned int unmortgagePrice() const { return m_unmortgagePrice; }
	void setColor(QColor color);
	QColor color() const { return m_color; }
	void setBgColor(QColor color);
	QColor bgColor() const { return m_bgColor; }
	void setPrice(unsigned int price);
	unsigned int price() const { return m_price; }
	void setMoney(int money);
	int money() const;
	void setIcon(const QString &icon);
	QString icon() const { return m_icon; }
	void update(bool force = false);

Q_SIGNALS:
	void changed();
	void estateToggleMortgage(Estate *estate);
	void estateHouseBuy(Estate *estate);
	void estateHouseSell(Estate *estate);
	void estateSell(Estate *estate);
	void newTrade(Player *player);
	void LMBClicked(Estate *estate);

protected:
	bool m_changed;
	int m_id;

private:
	QString m_name, m_icon;
	Player *m_owner;
	EstateGroup *m_estateGroup;
	unsigned int m_houses, m_price, m_housePrice, m_houseSellPrice, m_mortgagePrice, m_unmortgagePrice;
	int m_money;
	bool m_canBeOwned : 1, m_canBuyHouses : 1, m_canSellHouses : 1, m_isMortgaged : 1, m_canToggleMortgage : 1;
	QColor m_bgColor, m_color;
};

#endif
