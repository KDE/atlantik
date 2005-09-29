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

#include "player.h"
#include "player.moc"
#include "estate.h"

Player::Player(int playerId) : QObject()
{
	m_id = playerId;
	m_game = 0;
	m_name = "";
	m_host = "";
	m_image = "";
	m_location = m_destination = 0;
	m_money = 0;
	m_changed = m_isSelf = false;
	m_bankrupt = m_hasDebt = m_hasTurn = m_canRoll = m_canBuy = m_canAuction = m_canUseCard = m_inJail = false;
}

void Player::setGame(Game *game)
{
	if (m_game != game)
	{
		m_game = game;
		m_changed = true;
	}
}

Game *Player::game()
{
	return m_game;
}

void Player::setLocation(Estate *location)
{
	if (m_location != location)
	{
		m_location = location;
		m_changed = true;
	}
}

void Player::setDestination(Estate *destination)
{
	if (m_destination != destination)
	{
		m_destination = destination;
		m_changed = true;
	}
}

void Player::setBankrupt(bool bankrupt)
{
	if (m_bankrupt != bankrupt)
	{
		m_bankrupt = bankrupt;
		m_changed = true;
	}
}

void Player::setHasDebt(bool hasDebt)
{
	if (m_hasDebt != hasDebt)
	{
		m_hasDebt = hasDebt;
		m_changed = true;
	}
}

void Player::setHasTurn(const bool hasTurn)
{
	if (m_hasTurn != hasTurn)
	{
		m_hasTurn = hasTurn;
		m_changed = true;
		if (m_hasTurn && m_isSelf)
			emit gainedTurn();
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

void Player::setCanAuction(bool canAuction)
{
	if (m_canAuction != canAuction)
	{
		m_canAuction = canAuction;
		m_changed = true;
	}
}

void Player::setCanUseCard(bool canUseCard)
{
	if (m_canUseCard != canUseCard)
	{
		m_canUseCard = canUseCard;
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

void Player::setHost(const QString &host)
{
	if (m_host != host)
	{
		m_host = host;
		m_changed = true;
	}
}

void Player::setImage(const QString &image)
{
	if (m_image != image)
	{
		m_image = image;
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
