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

#include <qdom.h>
#include <qptrlist.h>
#include <qregexp.h>

#include <kextendedsocket.h>

#include "monopigator.moc"
#include "main.h"

Monopigator::Monopigator()
{
	m_downloadData = 0;
	m_job = 0;
	m_timer = 0;
}

Monopigator::~Monopigator()
{
	if (m_job)
		m_job -> kill();
        delete m_downloadData;
        m_downloadData=0L;
}

void Monopigator::loadData(const KURL &url)
{
    delete m_downloadData;
	m_downloadData = new QBuffer();
	m_downloadData->open(IO_WriteOnly);
	m_downloadData->reset();

	m_job = KIO::get(url, true, false);
	m_job->addMetaData(QString::fromLatin1("UserAgent"), QString::fromLatin1("Atlantik/" ATLANTIK_VERSION_STRING));

	if (!m_timer)
	{
		m_timer = new QTimer(this);
		m_timer->start(10000, true);
	}

	connect(m_job, SIGNAL(data(KIO::Job *, const QByteArray &)), SLOT(slotData(KIO::Job *, const QByteArray &)));
	connect(m_job, SIGNAL(result(KIO::Job *)), SLOT(slotResult(KIO::Job *)));
	connect(m_timer, SIGNAL(timeout()), SLOT(slotTimeout()));
}

void Monopigator::slotData(KIO::Job *, const QByteArray &data)
{
	m_timer->stop();
	m_downloadData->writeBlock(data.data(), data.size());
}

void Monopigator::slotResult(KIO::Job *job)
{
	processData(m_downloadData->buffer(), !job->error());
	m_job = 0;
}

void Monopigator::slotTimeout()
{
	if (m_job)
		m_job -> kill();
	m_job = 0;

	emit timeout();
}

void Monopigator::processData(const QByteArray &data, bool okSoFar)
{
	if (okSoFar)
	{
		QString xmlData(data);
		QDomDocument domDoc;
		if (domDoc.setContent(xmlData))
		{
			QDomElement eTop = domDoc.documentElement();
			if (eTop.tagName() != "monopigator")
				return;

			QDomNode n = eTop.firstChild();
			while(!n.isNull())
			{
				QDomElement e = n.toElement();
				if(!e.isNull())
				{
					if (e.tagName() == "server")
						emit monopigatorAdd(e.attributeNode(QString("ip")).value(), e.attributeNode(QString("host")).value(), e.attributeNode(QString("port")).value(), e.attributeNode(QString("version")).value(), e.attributeNode(QString("users")).value().toInt());
				}
				n = n.nextSibling();
			}
			emit finished();
		}
	}
}

MonopigatorEntry::MonopigatorEntry(QListView *parent, QString host, QString latency, QString version, QString users, QString port, QString ip) : QObject(), QListViewItem(parent, host, latency, version, users, port)
{
	m_isDev = ( version.find( QRegExp("(CVS|-dev)") ) != -1 ) ? true : false;

	setEnabled(false);
	parent->sort();

	if ( !ip.isEmpty() )
		host = ip;
	m_latencySocket = new KExtendedSocket( host, port.toInt(), KExtendedSocket::inputBufferedSocket | KExtendedSocket::noResolve );
	connect(m_latencySocket, SIGNAL(lookupFinished(int)), this, SLOT(resolved()));
	connect(m_latencySocket, SIGNAL(connectionSuccess()), this, SLOT(connected()));
	m_latencySocket->startAsyncConnect();
}

void MonopigatorEntry::resolved()
{
	time.start();
}

void MonopigatorEntry::connected()
{
	setText( 1, QString::number(time.elapsed()) );
	setEnabled(true);
	listView()->sort();
	delete m_latencySocket;
}

int MonopigatorEntry::compare(QListViewItem *i, int col, bool ascending) const
{
	// Colums 1 and 3 are integers (latency and users)
	if (col == 1 || col == 3)
	{
		int myVal = text(col).toInt(), iVal = i->text(col).toInt();
		if (myVal == iVal)
			return 0;
		else if (myVal > iVal)
			return 1;
		else
			return -1;
	}
	return key( col, ascending ).compare( i->key( col, ascending) );
}

bool MonopigatorEntry::isDev() const
{
	return m_isDev;
}

void MonopigatorEntry::showDevelopmentServers(bool show)
{
	if ( isVisible() != show )
		setVisible(show);
}
