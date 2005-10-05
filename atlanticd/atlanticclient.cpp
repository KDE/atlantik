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

#include <qtextstream.h>
#include <qtimer.h>

#include "atlanticclient.h"
#include "atlanticclient.moc"

AtlanticClient::AtlanticClient(QObject *parent, const char *name) : QSocket(parent, name)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

void AtlanticClient::sendData(const QString &data)
{
	writeBlock(data.latin1(), data.length());
}

void AtlanticClient::readData()
{
	if (canReadLine())
	{
		emit clientInput(this, readLine());

		// There might be more data
		QTimer::singleShot(0, this, SLOT(readData()));
	}
	else
	{
		// Maximum message size. Messages won't get bigger than 32k anyway, so
		// if we didn't receive a newline by now, we probably won't anyway.
		if (bytesAvailable() > (1024 * 32))
			flush();
	}
}
