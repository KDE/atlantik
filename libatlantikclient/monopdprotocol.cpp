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

#include <qstring.h>

/*
#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <estategroup.h>
#include <trade.h>
#include <auction.h>
*/

#include <estate.h>

#include "monopdprotocol.h"
#include "monopdprotocol.moc"

MonopdProtocol::MonopdProtocol() : QObject()
{
}

void MonopdProtocol::auctionEstate()
{
	sendData(QString::fromLatin1(".ea"));
}

void MonopdProtocol::buyEstate()
{
	sendData(QString::fromLatin1(".eb"));
}

void MonopdProtocol::confirmTokenLocation(Estate *estate)
{
	QString data(".t");
	data.append(QString::number(estate ? estate->id() : -1));
	sendData(data);
}

void MonopdProtocol::endTurn()
{
	sendData(QString::fromLatin1(".E"));
}

void MonopdProtocol::rollDice()
{
	sendData(QString::fromLatin1(".r"));
}

void MonopdProtocol::setName(QString name)
{
	QString data(".n");
	data.append(name);
	sendData(data);
}

void MonopdProtocol::startGame()
{
	sendData(QString::fromLatin1(".gs"));
}

void MonopdProtocol::sendData(QString)
{
	// Your reimplementation of this method should send send data over the
	// network.
}
