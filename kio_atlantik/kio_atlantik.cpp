#include <stdlib.h>
#include <qtextstream.h>

#include <kio/slavebase.h>
#include <kinstance.h>
#include <kdebug.h>
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
	QString host = url.queryItem("host");
	QString port = url.queryItem("port");
	QString game = url.queryItem("game");

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
