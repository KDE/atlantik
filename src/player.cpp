#include "player.moc"

Player::Player(int playerId) : QObject()
{
	m_playerId = playerId;
	m_isSelf = false;
}

void Player::setName(const QString _n)
{
	if (m_name != _n)
	{
		m_name = _n;
		m_portfolioView->updateName();
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
