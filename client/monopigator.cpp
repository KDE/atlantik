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

#include <qdom.h>

#include "monopigator.moc"
#include "main.h"

Monopigator::Monopigator()
{
	m_downloadData = 0;
}

void Monopigator::loadData(const KURL &url)
{
	if (m_downloadData)
		delete m_downloadData;
	m_downloadData = new QBuffer();
	m_downloadData->open(IO_WriteOnly);
	m_downloadData->reset();

	KIO::Job *job = KIO::get(url.url(), true, false);
	job->addMetaData(QString::fromLatin1("UserAgent"), QString::fromLatin1("Atlantik/" ATLANTIK_VERSION_STRING));

	connect(job, SIGNAL(data(KIO::Job *, const QByteArray &)), SLOT(slotData(KIO::Job *, const QByteArray &)));
	connect(job, SIGNAL(result(KIO::Job *)), SLOT(slotResult(KIO::Job *)));
}

void Monopigator::slotData(KIO::Job *, const QByteArray &data)
{
	m_downloadData->writeBlock(data.data(), data.size());
}

void Monopigator::slotResult(KIO::Job *job)
{
	processData(m_downloadData->buffer(), !job->error());
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

			emit monopigatorClear();

			QDomNode n = eTop.firstChild();
			while(!n.isNull())
			{
				QDomElement e = n.toElement();
				if(!e.isNull())
				{
					if (e.tagName() == "server")
						emit monopigatorAdd(e.attributeNode(QString("host")).value(), e.attributeNode(QString("port")).value(), e.attributeNode(QString("version")).value(), e.attributeNode(QString("users")).value().toInt());
				}
				n = n.nextSibling();
			}
			emit finished();
		}
	}
}
