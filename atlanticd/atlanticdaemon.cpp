// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <qtimer.h>
#include <qsocket.h>
#include <qstring.h>

#include <atlantic_core.h>

#include "atlanticdaemon.moc"
#include "serversocket.h"

AtlanticDaemon::AtlanticDaemon()
{
	m_serverSocket = new ServerSocket(1234, 100);
	m_atlanticCore = new AtlanticCore(this, "atlanticCore");

	// Create socket for Monopigator
	m_monopigatorSocket = new QSocket();
	connect(m_monopigatorSocket, SIGNAL(connected()), this, SLOT(monopigatorConnected()));

	// Register server
	monopigatorRegister();
}

void AtlanticDaemon::monopigatorRegister()
{
	m_monopigatorSocket->connectToHost("gator.monopd.net", 80);
}

void AtlanticDaemon::monopigatorConnected()
{
	QString get;
	get = "GET /register.php?host=capsi.com&port=1234&version=atlanticd-prototype HTTP/1.1\nHost: gator.monopd.net\n\n";

	m_monopigatorSocket->writeBlock(get.latin1(), get.length());
	m_monopigatorSocket->close();

	// Monopigator clears old entries, so keep registering every 90s
	QTimer::singleShot(90000, this, SLOT(monopigatorRegister()));
}
