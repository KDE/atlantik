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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef LIBATLANTIC_PLAYER_H
#define LIBATLANTIC_PLAYER_H

#include <qobject.h>
#include <qstring.h>

class Estate;

class Player : public QObject
{
Q_OBJECT

public:
	Player(int playerId);

	int id() { return m_id; }
	void setGame(int gameId);
	int gameId() { return m_gameId; }
	void setLocation(Estate *estate);
	Estate *location() { return m_location; }
	void setDestination(Estate *estate);
	Estate *destination() { return m_destination; }
	void setIsSelf(const bool isSelf) { m_isSelf = isSelf; }
	bool isSelf() const { return m_isSelf; }
	void setMaster(bool master);
	bool master() { return m_master; }
	void setBankrupt(bool bankrupt);
	bool isBankrupt() { return m_bankrupt; }
	void setHasDebt(bool hasDebt);
	bool hasDebt() { return m_hasDebt; }
	void setHasTurn(const bool hasTurn);
	bool hasTurn() const { return m_hasTurn; }
	void setCanRoll(bool canRoll);
	bool canRoll() const { return m_canRoll; }
	void setCanBuy(bool canBuy);
	bool canBuy() const { return m_canBuy; }
	void setInJail(const bool inJail);
	bool inJail() const { return m_inJail; }
	void setName(const QString _n);
	QString name() const { return m_name; }
	void setHost(const QString &host);
	QString host() const { return m_host; }
	void setImage(const QString &image);
	const QString image() const { return m_image; }
	void setMoney(unsigned int _m);
	unsigned int money() const { return m_money; }
	void update(bool force = false);

signals:
	void changed(Player *player);

private:
	int m_id, m_gameId;
	bool m_changed, m_isSelf;
	bool m_master, m_bankrupt, m_hasDebt, m_hasTurn, m_canRoll, m_canBuy, m_inJail;
	unsigned int m_money;
	QString m_name, m_host, m_image;
	Estate *m_location, *m_destination;
};

#endif
