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
#include <qlistview.h>
#include <qtimer.h>

#include <kio/job.h>
#include <kurl.h>

class KLatencyTimer;

class Monopigator : public QObject
{
Q_OBJECT

public:
	Monopigator();
	~Monopigator();
	void loadData(const KURL &);

signals:
	void monopigatorAdd(QString host, QString port, QString version, int users);
	void finished();
	void timeout();

private slots:
	void slotData(KIO::Job *, const QByteArray &);
	void slotResult(KIO::Job *);
	void slotTimeout();

private:
	void processData(const QByteArray &, bool = true);

	QBuffer *m_downloadData;
	QTimer *m_timer;
	KIO::Job *m_job;
};

class MonopigatorEntry : public QObject, public QListViewItem
{
Q_OBJECT

public:
	MonopigatorEntry(QListView *parent, QString host, QString latency, QString version, QString users, QString port);
	int compare(QListViewItem *i, int col, bool ascending) const;

private slots:
	void updateLatency(int msec);

private:
	KLatencyTimer *m_latencyTimer;
};

#endif
