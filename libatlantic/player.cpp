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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include "player.h"
#include "player.moc"
#include "estate.h"

Player::Player(int playerId) : QObject()
{
	m_id = playerId;
	m_location = 0;
	m_money = 0;
	m_changed = m_isSelf = false;
	m_hasTurn = m_canRoll = m_canBuy = m_inJail = false;
}

void Player::setLocation(Estate *location)
{
	if (m_location != location)
	{
		m_location = location;
		m_changed = true;
	}
}

void Player::setHasTurn(const bool hasTurn)
{
	if (m_hasTurn != hasTurn)
	{
		m_hasTurn = hasTurn;
		m_changed = true;
	}
}

void Player::setCanRoll(bool canRoll)
{
	if (m_canRoll != canRoll)
	{
		m_canRoll = canRoll;
		m_changed = true;
	}
}

void Player::setCanBuy(bool canBuy)
{
	if (m_canBuy != canBuy)
	{
		m_canBuy = canBuy;
		m_changed = true;
	}
}

void Player::setInJail(const bool inJail)
{
	if (m_inJail != inJail)
	{
		m_inJail = inJail;
		m_changed = true;
	}
}

void Player::setName(const QString _n)
{
	if (m_name != _n)
	{
		m_name = _n;
		m_changed = true;
	}
}

void Player::setMoney(unsigned int money)
{
	if (m_money != money)
	{
		m_money = money;
		m_changed = true;
	}
}

void Player::update(bool force)
{
	if (m_changed || force)
	{
		emit changed(this);
		m_changed = false;
	}
}
