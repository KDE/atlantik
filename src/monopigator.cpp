#include <qdom.h>

#include "monopigator.moc"

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
						emit monopigatorAdd(e.attributeNode(QString("host")).value(), e.attributeNode(QString("port")).value(), e.attributeNode(QString("version")).value());
				}
				n = n.nextSibling();
			}
			emit finished();
		}
	}
}
