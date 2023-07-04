// Copyright (c) 2015 Pino Toscano <pino@kde.org>
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

#include "card.h"
#include "player.h"

Card::Card(int cardId)
	: QObject()
	, m_changed(false)
	, m_cardId(cardId)
	, m_owner(nullptr)
{
}

Card::~Card()
{
}

void Card::setTitle(const QString &title)
{
	if (m_title != title)
	{
		m_title = title;
		m_changed = true;
	}
}

void Card::setOwner(Player *player)
{
	if (m_owner != player)
	{
		m_owner = player;
		m_changed = true;
	}
}

void Card::update(bool force)
{
	if (m_changed || force)
	{
		Q_EMIT changed(this);
		m_changed = false;
	}
}

#include "moc_card.cpp"
