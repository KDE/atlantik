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

#include "connectioncookie.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

static const int ConnectionCookieVersion = 1;

static QString filePath()
{
	return QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QStringLiteral("/atlantik.cookie");
}

ConnectionCookie::ConnectionCookie(const QString &host, int port, const QString &cookie)
	: m_host(host)
	, m_port(port)
	, m_cookie(cookie)
{
	QFile f(filePath());
	QDir().mkpath(QFileInfo(f).absolutePath());
	if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return;

	QDataStream out(&f);
	out.setVersion(QDataStream::Qt_4_6);
	out << ConnectionCookieVersion;
	out << m_host;
	out << m_port;
	out << m_cookie;
}

ConnectionCookie::ConnectionCookie() : m_port(-1)
{
}

ConnectionCookie::~ConnectionCookie()
{
	QFile::remove(filePath());
}

ConnectionCookie *ConnectionCookie::read()
{
	QFile f(filePath());
	if (!f.exists())
		return nullptr;
	if (!f.open(QIODevice::ReadOnly))
		return nullptr;

	QDataStream in(&f);
	in.setVersion(QDataStream::Qt_4_6);
	int version;
	in >> version;
	if (version != ConnectionCookieVersion)
		return nullptr;

	ConnectionCookie *cookie = new ConnectionCookie();
	in >> cookie->m_host;
	in >> cookie->m_port;
	in >> cookie->m_cookie;
	return cookie;
}
