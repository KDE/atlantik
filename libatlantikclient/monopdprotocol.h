// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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

// WARNING: this codebase is not being used yet. Please use AtlantikNetwork
// until the protocol seperation has been completed.

#ifndef MONOPDPROTOCOL_H_H
#define MONOPDPROTOCOL_H_H

#include <qobject.h>

class QString;

/*
class AtlanticCore;

class Player;
class EstateGroup;
class Trade;
class Auction;
*/

class Estate;

class MonopdProtocol : public QObject
{
Q_OBJECT

public:
	MonopdProtocol();

private slots:
	void auctionEstate();
	void buyEstate();
	void confirmTokenLocation(Estate *estate);
	void endTurn();
	void rollDice();
	void setName(QString name);
	void startGame();

private:
	virtual void sendData(QString data);
};

#endif
