//#include <kdebug.h>

#include "player.h"
#include "player.moc"
#include "estate.h"

Player::Player(int playerId) : QObject()
{
	m_playerId = playerId;
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

void Player::setMoney(unsigned int _m)
{
	if (m_money != _m)
	{
		m_money = _m;
		m_changed = true;
	}
}

void Player::update(bool force)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}
