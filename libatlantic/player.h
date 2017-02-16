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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef LIBATLANTIC_PLAYER_H
#define LIBATLANTIC_PLAYER_H

#include <QObject>

#include "libatlantic_export.h"

class Estate;
class Game;

class LIBATLANTIC_EXPORT Player : public QObject
{
Q_OBJECT

public:
	Player(int playerId);

	int id() const { return m_id; }
	void setGame(Game *game);
	Game *game() const;
	void setLocation(Estate *location);
	Estate *location() const { return m_location; }
	void setDestination(Estate *destination);
	Estate *destination() const { return m_destination; }
	void setIsSelf(bool isSelf) { m_isSelf = isSelf; }
	bool isSelf() const { return m_isSelf; }
	void setBankrupt(bool bankrupt);
	bool isBankrupt() const { return m_bankrupt; }
	void setHasDebt(bool hasDebt);
	bool hasDebt() const { return m_hasDebt; }
	void setHasTurn(bool hasTurn);
	bool hasTurn() const { return m_hasTurn; }
	void setCanRoll(bool canRoll);
	bool canRoll() const { return m_canRoll; }
	void setCanBuy(bool canBuy);
	bool canBuy() const { return m_canBuy; }
	void setCanAuction(bool canAuction);
	bool canAuction() const { return m_canAuction; }
	void setCanUseCard(bool canUseCard);
	bool canUseCard() const { return m_canUseCard; }
	void setInJail(bool inJail);
	bool inJail() const { return m_inJail; }
	void setName(const QString &_n);
	QString name() const { return m_name; }
	void setHost(const QString &host);
	QString host() const { return m_host; }
	void setImage(const QString &image);
	const QString image() const { return m_image; }
	void setMoney(unsigned int _m);
	unsigned int money() const { return m_money; }
	void setSpectator(bool spectator);
	bool isSpectator() const { return m_spectator; }
	void update(bool force = false);

Q_SIGNALS:
	void changed(Player *player);
	void gainedTurn();

private:
	int m_id;
	bool m_changed : 1, m_isSelf : 1;
	bool m_bankrupt : 1, m_hasDebt : 1, m_hasTurn : 1, m_canRoll : 1, m_canBuy : 1, m_canAuction : 1, m_canUseCard : 1, m_inJail : 1;
	bool m_spectator : 1;
	unsigned int m_money;
	QString m_name, m_host, m_image;
	Game *m_game;
	Estate *m_location, *m_destination;
};

#endif
