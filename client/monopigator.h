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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef ATLANTIK_MONOPIGATOR_H
#define ATLANTIK_MONOPIGATOR_H

#include <qobject.h>
#include <qbuffer.h>

#include <kio/job.h>
#include <kurl.h>

class Monopigator : public QObject
{
Q_OBJECT

public:
	Monopigator();
	void loadData(const KURL &);

signals:
	void monopigatorAdd(QString host, QString port, QString version, int users);
	void finished();

private slots:
	void slotData(KIO::Job *, const QByteArray &);
	void slotResult(KIO::Job *);

private:
	void processData(const QByteArray &, bool = true);

	QBuffer *m_downloadData;
};

#endif
