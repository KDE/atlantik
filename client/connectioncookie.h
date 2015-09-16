// Copyright (c) 2015 Pino Toscano <pino@kde.org>
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

#ifndef ATLANTIK_CCONNECTIONCOOKIE_H
#define ATLANTIK_CCONNECTIONCOOKIE_H

#include <QString>

class ConnectionCookie
{
public:
	ConnectionCookie(const QString &host, int port, const QString &cookie);
	~ConnectionCookie();

	QString host() const { return m_host; }
	int port() const { return m_port; }
	QString cookie() const { return m_cookie; }

	static ConnectionCookie *read();

private:
	ConnectionCookie();

	QString m_host;
	int m_port;
	QString m_cookie;
};

#endif
