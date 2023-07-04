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

MonopdProtocol::MonopdProtocol()
	: QObject()
{
}

void MonopdProtocol::auctionEstate()
{
	sendData(QStringLiteral(".ea"));
}

void MonopdProtocol::buyEstate()
{
	sendData(QStringLiteral(".eb"));
}

void MonopdProtocol::confirmTokenLocation(Estate *estate)
{
	QString data(QStringLiteral(".t"));
	data.append(QString::number(estate ? estate->id() : -1));
	sendData(data);
}

void MonopdProtocol::endTurn()
{
	sendData(QStringLiteral(".E"));
}

void MonopdProtocol::rollDice()
{
	sendData(QStringLiteral(".r"));
}

void MonopdProtocol::setName(const QString &name)
{
	QString data(QStringLiteral(".n"));
	data.append(name);
	sendData(data);
}

void MonopdProtocol::startGame()
{
	sendData(QStringLiteral(".gs"));
}

void MonopdProtocol::sendData(const QString&)
{
	// Your reimplementation of this method should send send data over the
	// network.
}

#include "moc_monopdprotocol.cpp"
