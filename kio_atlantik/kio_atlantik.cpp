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

#include <stdlib.h>

#include <qtextstream.h>

#include <kdeversion.h>
#include <kio/slavebase.h>
#include <kinstance.h>
#include <kprocess.h>

#include "kio_atlantik.h"
#include "libatlantic_export.h"

extern "C"
{
	int KDE_EXPORT kdemain( int, char **argv )
	{
		KInstance instance( "kio_atlantik" );
		AtlantikProtocol slave(argv[2], argv[3]);
		slave.dispatchLoop();
		return 0;
	}
}

void AtlantikProtocol::get( const KUrl& url )
{
	KProcess *proc = new KProcess;
	*proc << "atlantik";

	QString host = url.hasHost() ? url.host() : KProcess::quote( url.queryItem("host") );
	QString port = QString::number( url.port() ? url.port() : 1234 );
	int game = url.queryItem("game").toInt();
	QString gameString = game ? QString::number( game ) : QString::null;

	if (!host.isNull() && !port.isNull())
	{
		*proc << "--host" << host << "--port" << port;
		if (!gameString.isNull())
			*proc << "--game" << gameString;
	}

	proc->start(KProcess::DontCare);
	proc->detach();
	finished();
}
