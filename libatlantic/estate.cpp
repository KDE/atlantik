// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License version 2 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

//#include <kdebug.h>

#include "estate.h"
#include "estate.moc"
#include "player.h"

Estate::Estate(int estateId) : QObject()
{
	m_estateId = estateId;
	m_owner = 0;
	m_houses = 0;
	m_changed = m_canBeOwned = m_canBuyHouses = m_canSellHouses = m_isMortgaged = m_canToggleMortgage = false;
	m_bgColor = QColor();
	m_color = QColor();
}

void Estate::setGroup(const QString group)
{
	if (m_group != group)
		m_group = group;
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
		m_houses = houses;
		m_changed = true;
}

void Estate::setName(const QString name)
{
	if (m_name != name)
	{
		m_name = name;
		m_changed = true;
	}
}

void Estate::setColor(const QColor color)
{
	if (m_color != color)
	{
		m_color = color;
		m_changed = true;
	}
}

void Estate::setBgColor(const QColor color)
{
	if (m_bgColor != color)
	{
		m_bgColor = color;
		m_changed = true;
	}
}

void Estate::setCanBeOwned(const bool canBeOwned)
{
	if (m_canBeOwned != canBeOwned)
		m_canBeOwned = canBeOwned;
}

void Estate::setCanBuyHouses(const bool canBuyHouses)
{
	if (m_canBuyHouses != canBuyHouses)
		m_canBuyHouses = canBuyHouses;
}

void Estate::setCanSellHouses(const bool canSellHouses)
{
	if (m_canSellHouses != canSellHouses)
		m_canSellHouses = canSellHouses;
}

void Estate::setIsMortgaged(const bool isMortgaged)
{
	if (m_isMortgaged != isMortgaged)
	{
		m_isMortgaged = isMortgaged;
		m_changed = true;
	}
}

void Estate::setCanToggleMortgage(const bool canToggleMortgage)
{
	if (m_canToggleMortgage != canToggleMortgage)
		m_canToggleMortgage = canToggleMortgage;
}

void Estate::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}
