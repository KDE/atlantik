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

#ifndef LIBATLANTIC_CARD_H
#define LIBATLANTIC_CARD_H

#include <QObject>

#include "libatlantic_export.h"

class Player;

class LIBATLANTIC_EXPORT Card : public QObject
{
Q_OBJECT

public:
	Card(int cardId);
	virtual ~Card();

	int cardId() const { return m_cardId; }

	void setTitle(const QString &title);
	QString title() const { return m_title; }

	void setOwner(Player *player);
	Player *owner() const { return m_owner; }
	bool isOwned() const { return m_owner != nullptr; }

	void update(bool force = false);

Q_SIGNALS:
	void changed(Card *card);

private:
	bool m_changed;
	int m_cardId;
	QString m_title;
	Player *m_owner;
};

#endif
