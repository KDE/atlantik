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

#include <QObject>
#include <qbuffer.h>
#include <q3listview.h>
#include <QTimer>
#include <QTime>
#include <k3bufferedsocket.h>
#include <kio/job.h>
#include <kurl.h>

class KNetwork::KBufferedSocket;
class QTime;

class Monopigator : public QObject
{
Q_OBJECT

public:
	Monopigator();
	~Monopigator();
	void loadData(const KUrl &);

signals:
	void monopigatorAdd(QString ip, QString host, QString port, QString version, int users);
	void finished();
	void timeout();

private slots:
	void slotData(KIO::Job *, const QByteArray &);
	void slotResult(KJob *);
	void slotTimeout();

private:
	void processData(const QByteArray &, bool = true);

	QBuffer *m_downloadData;
	QTimer *m_timer;
	KIO::Job *m_job;
};


class MonopigatorEntry : public QObject, public Q3ListViewItem
{
Q_OBJECT

public:
	MonopigatorEntry(Q3ListView *parent, const QString &host, const QString &latency, const QString &version, const QString &users, const QString &port, const QString &ip);
	int compare(Q3ListViewItem *i, int col, bool ascending) const;
	bool isDev() const;

private slots:
	void resolved();
	void connected();
	void showDevelopmentServers(bool show);

private:
	KNetwork::KBufferedSocket *m_latencySocket;
	QTime time;
	Q3ListView *m_parent;
	bool m_isDev;
};

#endif
