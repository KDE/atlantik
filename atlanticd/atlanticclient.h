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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef CLIENT_H
#define CLIENT_H

#include <qsocket.h>

class AtlanticClient : public QSocket
{
Q_OBJECT

public:
	AtlanticClient(QObject *parent = 0, const char *name = 0);
	void sendData(const QString &data);

private slots:
	void readData();

signals:
	void clientInput(AtlanticClient *client, const QString &data);
};
#endif
