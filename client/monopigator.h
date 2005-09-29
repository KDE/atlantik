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

#ifndef ATLANTIK_MONOPIGATOR_H
#define ATLANTIK_MONOPIGATOR_H

#include <qobject.h>
#include <qbuffer.h>
#include <qlistview.h>
#include <qtimer.h>

#include <kio/job.h>
#include <kurl.h>

class KExtendedSocket;
class QTime;

class Monopigator : public QObject
{
Q_OBJECT

public:
	Monopigator();
	~Monopigator();
	void loadData(const KURL &);

signals:
	void monopigatorAdd(QString ip, QString host, QString port, QString version, int users);
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
	MonopigatorEntry(QListView *parent, QString host, QString latency, QString version, QString users, QString port, QString ip);
	int compare(QListViewItem *i, int col, bool ascending) const;
	bool isDev() const;

private slots:
	void resolved();
	void connected();
	void showDevelopmentServers(bool show);

private:
	KExtendedSocket *m_latencySocket;
	QTime time;
	QListView *m_parent;
	bool m_isDev;
};

#endif
