// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include "atlanticclient.h"
#include "serversocket.h"

ServerSocket::ServerSocket(int port, int backlog) : QServerSocket(port, backlog)
{
}

void ServerSocket::newConnection(int socket)
{
	AtlanticClient *client = new AtlanticClient(this, "socket");
	client->setSocket(socket);

	emit newClient(client);
}

#include "serversocket.moc"
