// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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

#include "kio_atlantik.h"

#include <QCoreApplication>
#include <QUrlQuery>

#include <kprocess.h>

#include <monopd.h>

extern "C"
{
	int Q_DECL_EXPORT kdemain(int argc, char **argv)
	{
		QCoreApplication app(argc, argv);
		app.setApplicationName(QLatin1String("kio_atlantik"));

		AtlantikProtocol slave(argv[2], argv[3]);
		slave.dispatchLoop();
		return 0;
	}
}

AtlantikProtocol::AtlantikProtocol(const QByteArray &pool, const QByteArray &app)
	: KIO::SlaveBase("atlantik", pool, app)
{
}

void AtlantikProtocol::get( const QUrl& url )
{
	QStringList args;
	const QUrlQuery query( url );
	args << QStringLiteral("atlantik");

	QString host = url.host();
	if (host.isEmpty())
		host = query.queryItemValue(QStringLiteral("host"));
	int game = query.queryItemValue(QStringLiteral("game")).toInt();
	QString gameString = game ? QString::number( game ) : QString();

	if (!host.isEmpty())
	{
		const QString port = QString::number(url.port(MONOPD_PORT));
		args << QStringLiteral("--host") << host << QStringLiteral("--port") << port;
		if (!gameString.isEmpty())
			args << QStringLiteral("--game") << gameString;
	}

	// TODO: check the return value?
	KProcess::startDetached(args);
	finished();
}
