// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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

#include "estate.h"
#include "player.h"

Estate::Estate(int estateId)
	: QObject()
	, m_changed(false)
	, m_id(estateId)
	, m_owner(nullptr)
	, m_estateGroup(nullptr)
	, m_houses(0)
	, m_price(0)
	, m_housePrice(0)
	, m_houseSellPrice(0)
	, m_mortgagePrice(0)
	, m_unmortgagePrice(0)
	, m_money(0)
	, m_canBeOwned(false)
	, m_canBuyHouses(false)
	, m_canSellHouses(false)
	, m_isMortgaged(false)
	, m_canToggleMortgage(false)
{
}

void Estate::setEstateGroup(EstateGroup *estateGroup)
{
	if (m_estateGroup != estateGroup)
	{
		m_estateGroup = estateGroup;
		m_changed = true;
	}
}

void Estate::setOwner(Player *player)
{
	if (m_owner != player)
	{
		m_owner = player;
		m_changed = true;
	}
}
bool Estate::isOwned() const
{
	if (m_owner)
		return true;
	else
		return false;
}

bool Estate::isOwnedBySelf() const
{
	if (m_owner && m_owner->isSelf())
		return true;
	else
		return false;
}

void Estate::setHouses(unsigned int houses)
{
	if (m_houses != houses)
	{
		m_houses = houses;
		m_changed = true;
	}
}

void Estate::setName(const QString& name)
{
	if (m_name != name)
	{
		m_name = name;
		m_changed = true;
	}
}

QString Estate::name() const
{
	return m_name;
}

void Estate::setColor(QColor color)
{
	if (m_color != color)
	{
		m_color = color;
		m_changed = true;
	}
}

void Estate::setBgColor(QColor color)
{
	if (m_bgColor != color)
	{
		m_bgColor = color;
		m_changed = true;
	}
}

void Estate::setCanBeOwned(bool canBeOwned)
{
	if (m_canBeOwned != canBeOwned)
	{
		m_canBeOwned = canBeOwned;
		m_changed = true;
	}
}

void Estate::setCanBuyHouses(bool canBuyHouses)
{
	if (m_canBuyHouses != canBuyHouses)
	{
		m_canBuyHouses = canBuyHouses;
		m_changed = true;
	}
}

void Estate::setCanSellHouses(bool canSellHouses)
{
	if (m_canSellHouses != canSellHouses)
	{
		m_canSellHouses = canSellHouses;
		m_changed = true;
	}
}

void Estate::setHousePrice(unsigned int housePrice)
{
	if (m_housePrice != housePrice)
	{
		m_housePrice = housePrice;
		m_changed = true;
	}
}

void Estate::setHouseSellPrice(unsigned int houseSellPrice)
{
	if (m_houseSellPrice != houseSellPrice)
	{
		m_houseSellPrice = houseSellPrice;
		m_changed = true;
	}
}

void Estate::setIsMortgaged(bool isMortgaged)
{
	if (m_isMortgaged != isMortgaged)
	{
		m_isMortgaged = isMortgaged;
		m_changed = true;
	}
}

void Estate::setCanToggleMortgage(bool canToggleMortgage)
{
	if (m_canToggleMortgage != canToggleMortgage)
	{
		m_canToggleMortgage = canToggleMortgage;
		m_changed = true;
	}
}

void Estate::setMortgagePrice(unsigned int mortgagePrice)
{
	if (m_mortgagePrice != mortgagePrice)
	{
		m_mortgagePrice = mortgagePrice;
		m_changed = true;
	}
}

void Estate::setUnmortgagePrice(unsigned int unmortgagePrice)
{
	if (m_unmortgagePrice != unmortgagePrice)
	{
		m_unmortgagePrice = unmortgagePrice;
		m_changed = true;
	}
}

void Estate::setPrice(unsigned int price)
{
	if (m_price != price)
	{
		m_price = price;
		m_changed = true;
	}
}

void Estate::setMoney(int money)
{
	if (m_money != money)
	{
		m_money = money;
		m_changed = true;
	}
}

int Estate::money() const
{
	return m_money;
}

void Estate::setIcon(const QString &icon)
{
	if (m_icon != icon)
	{
		m_icon = icon;
		m_changed = true;
	}
}

void Estate::update(bool force)
{
	if (m_changed || force)
	{
		Q_EMIT changed();
		m_changed = false;
	}
}

#include "moc_estate.cpp"
