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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef	ATLANTIC_ATLANTICDAEMON_H
#define	ATLANTIC_ATLANTICDAEMON_H

#include <qptrlist.h>

class QSocket;

class AtlanticCore;
class AtlanticClient;
class ServerSocket;

class AtlanticDaemon : public QObject
{
Q_OBJECT

public:
	AtlanticDaemon();
    ~AtlanticDaemon();
private slots:
	void monopigatorRegister();
	void monopigatorConnected();
	void newClient(AtlanticClient *client);
	void clientInput(AtlanticClient *client, const QString &data);

private:
	QSocket *m_monopigatorSocket;
	ServerSocket *m_serverSocket;
	AtlanticCore *m_atlanticCore;
	QPtrList<AtlanticClient> m_clients;
};

#endif
