// Copyright (c) 2015 Pino Toscano <pino@kde.org>
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

#include "metatlantic.h"
#include "metatlantic.moc"

#include <QDomDocument>
#include <QTcpSocket>
#include <QTimer>

#include <kdebug.h>

Metatlantic::Metatlantic()
{
	m_socket = 0;
}

Metatlantic::~Metatlantic()
{
}

void Metatlantic::loadData(const QString &host, int port)
{
	m_socket = new QTcpSocket(this);
	m_socket->setSocketOption(QAbstractSocket::LowDelayOption, true);
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(connected()), this, SLOT(slotSocketConnected()));

	m_socket->connectToHost(host, port);
}

void Metatlantic::slotSocketError(QAbstractSocket::SocketError socketError)
{
	if (socketError == QAbstractSocket::SocketTimeoutError)
		emit timeout();
}

void Metatlantic::slotSocketConnected()
{
	m_stream.setCodec(QTextCodec::codecForName("UTF-8"));
	m_stream.setDevice(m_socket);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));
}

void Metatlantic::slotSocketRead()
{
	const QString line = m_stream.readLine();
	if (!line.isNull())
	{
		processMsg(line);
		QTimer::singleShot(0, this, SLOT(slotSocketRead()));
	}
}

void Metatlantic::closeSocket()
{
	m_stream.setDevice(0);
	m_socket->close();
	delete m_socket;
	m_socket = 0;
	emit finished();
}

void Metatlantic::processMsg(const QString &msg)
{
	kDebug() << msg <<endl;
	QDomDocument dom;
	dom.setContent(msg);
	QDomElement e = dom.documentElement();
	if (e.tagName() != "meta_atlantic") {
		// Invalid data, close the connection
		closeSocket();
		return;
	}
	QDomNode n = e.firstChild();
	QDomAttr a;
	bool do_send_follow = false;
	bool do_close = false;
	for (; !n.isNull() ; n = n.nextSibling()) {
		QDomElement e = n.toElement();
		if (!e.isNull())
		{
			if (e.tagName() == "metaserver")
			{
				a = e.attributeNode("version");
				if (!a.isNull())
				{
					const QString serverVersion = a.value();
					kDebug() << "metaserver version" << serverVersion;
				}
				do_send_follow = true;
			}
			else if (e.tagName() == "server")
			{
				const QString host = e.attributeNode("host").value();
				const int port = e.attributeNode("port").value().toInt();
				const QString version = e.attributeNode("version").value();
				const int users = e.attributeNode("users").value().toInt();
				emit metatlanticAdd(host, port, version, users);
				do_close = true;
			} else
				kDebug() << "ignored TAG: " << e.tagName() << endl;
		}
	}
	if (do_send_follow)
		m_stream << "FOLLOW" << endl;
	if (do_close)
		closeSocket();
}