#include "estate.h"
#include "player.h"

Estate::Estate(int estateId) : QObject()
{
	m_estateId = estateId;
	m_owner = 0;
	m_houses = 0;
	m_canBeOwned = m_isMortgaged = m_canToggleMortgage = false;
	m_bgColor = QColor();
}

void Estate::setOwner(Player *player)
{
	if (m_owner != player)
	{
		m_owner = player;
#warning emit signal changed here
//		m_estateView->setOwned( (m_owner ? true : false), ( (m_owner && m_owner->isSelf() ) ? true : false) );
	}
}

void Estate::setHouses(unsigned int houses)
{
	if (m_houses != houses)
		m_houses = houses;
#warning emit signal changed here
//		m_estateView->redraw();
}

void Estate::setName(const QString name)
{
	if (m_name != name)
	{
		m_name = name;
#warning emit signal changed here
		// TODO: Update view? Done?
//		m_estateView->redraw();
	}
}

void Estate::setBgColor(const QColor color)
{
	if (m_bgColor != color)
	{
		m_bgColor = color;
#warning emit signal changed here
//		m_estateView->redraw();
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
		// TODO: Update view? Done?
#warning emit signal changed here
//		m_estateView->redraw();
	}
}

void Estate::setCanToggleMortgage(const bool canToggleMortgage)
{
	if (m_canToggleMortgage != canToggleMortgage)
		m_canToggleMortgage = canToggleMortgage;
}
