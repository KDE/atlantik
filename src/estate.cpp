#include <kdebug.h>

#include "estate.h"
#include "player.h"

Estate::Estate(int estateId) : QObject()
{
	m_estateId = estateId;
	m_owner = 0;
	m_houses = 0;
	m_canBeOwned = m_isMortgaged = m_canToggleMortgage = false;
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
	{
		m_canBeOwned = canBeOwned;
		// TODO: Update view?
	}
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

void Estate::update(bool force = false)
{
	if (m_changed || force)
	{
		emit changed();
		m_changed = false;
	}
}