#include <kdebug.h>

#include "estate.h"
#include "estate.moc"
#include "player.h"

Estate::Estate(int estateId) : QObject()
{
	m_estateId = estateId;
	m_owner = 0;
	m_houses = 0;
	m_canBeOwned = m_canBuyHouses = m_canSellHouses = m_isMortgaged = m_canToggleMortgage = false;
	m_bgColor = QColor();
	m_color = QColor();
}

void Estate::setOwner(Player *player)
{
	if (m_owner != player)
	{
		m_owner = player;
		m_changed = true;
	}
}
const bool Estate::owned()
{
	if (m_owner)
		return true;
	else
		return false;
}

const bool Estate::ownedBySelf()
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
