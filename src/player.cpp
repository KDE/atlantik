#include "player.h"

Player::Player(uint id)
{
	m_id = id;
}

void Player::setName(const QString _n)
{
	if (m_name != _n)
	{
		m_name = _n;
		m_portfolioView->setName(m_name);
	}
}

void Player::setMoney(const QString _m)
{
	if (m_money != _m)
	{
		m_money = _m;
		m_portfolioView->setMoney(m_money);
	}
}
