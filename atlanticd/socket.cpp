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

#include <qtextstream.h>
#include <qtimer.h>

#include "socket.h"
#include "socket.moc"

Socket::Socket(QObject *parent, const char *name) : QSocket(parent, name)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

void Socket::readData()
{
	if (canReadLine())
	{
		char *tmp = new char[1024 * 32];
		readLine(tmp, 1024 * 32);
		// processMsg(tmp);
		delete[] tmp;

		// There might be more data
		QTimer::singleShot(0, this, SLOT(slotRead()));
	}
	else
	{
		// Maximum message size. Messages won't get bigger than 32k anyway, so
		// if we didn't receive a newline by now, we probably won't anyway.
		if (bytesAvailable() > (1024 * 32))
			flush();
	}
}
