#include <kdebug.h>

#include "player.moc"

Player::Player(int playerId) : QObject()
{
	m_playerId = playerId;
	m_changed = m_isSelf = m_hasTurn = false;
}

void Player::setLocation(const int estateId)
{
	kdDebug() << "Player::setLocation" << endl;
	if (m_location != estateId)
	{
		m_location = estateId;
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

void Player::setMoney(const QString _m)
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
