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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <stdlib.h>
#include <qtextstream.h>

#include <kio/slavebase.h>
#include <kinstance.h>
#include <kdebug.h>
#include <kdeversion.h>
#undef KDE_3_1_FEATURES
#ifdef KDE_MAKE_VERSION
#if KDE_VERSION > KDE_MAKE_VERSION (3, 1, 0)
#define KDE_3_1_FEATURES
#endif
#endif
#include <kprocess.h>

#include "kio_atlantik.h"

extern "C"
{
	int kdemain( int, char **argv )
	{
		KInstance instance( "kio_atlantik" );
		AtlantikProtocol slave(argv[2], argv[3]);
		slave.dispatchLoop();
		return 0;
	}
}

void AtlantikProtocol::get( const KURL& url )
{
	KProcess *proc = new KProcess;
	*proc << "atlantik";

#ifdef KDE_3_1_FEATURES
	QString host = KProcess::quote( url.hasHost() ? url.host() : url.queryItem("host") );
	QString game = KProcess::quote(url.queryItem("game"));
#else
	QString host = url.hasHost() ? url.host() : url.queryItem("host");
	QString game = url.queryItem("game"));
#endif
	QString port = QString::number( url.port() ? url.port() : 1234 );

	if (!host.isNull() && !port.isNull())
	{
		*proc << "--host" << host << "--port" << port;
		if (!game.isNull())
			*proc << "--game" << game;
	}

	proc->start(KProcess::DontCare);
	proc->detach();
	finished();
}
