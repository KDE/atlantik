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

#include <QTcpSocket>
#include <QTimer>
#include <QXmlStreamReader>

#include <atlantik_debug.h>

Metatlantic::Metatlantic(const QString &host, int port, QObject *parent)
	: KJob(parent)
	, m_host(host)
	, m_port(port)
	, m_socket(nullptr)
{
	setCapabilities(Killable);
}

Metatlantic::~Metatlantic()
{
	if (m_socket)
		closeSocket(false);
}

void Metatlantic::start()
{
	m_socket = new QTcpSocket(this);
	m_socket->setSocketOption(QAbstractSocket::LowDelayOption, true);
	connect(m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(connected()), this, SLOT(slotSocketConnected()));

	m_socket->connectToHost(m_host, m_port);
}

bool Metatlantic::doKill()
{
	closeSocket(false);
	return true;
}

void Metatlantic::slotSocketError(QAbstractSocket::SocketError socketError)
{
	setError(UserDefinedError + socketError);
	setErrorText(m_socket->errorString());
	emitResult();
}

void Metatlantic::slotSocketConnected()
{
	m_stream.setDevice(m_socket);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));
}

void Metatlantic::slotSocketRead()
{
	if (!m_stream.device())
		return;

	const QString line = m_stream.readLine();
	if (!line.isNull())
	{
		processMsg(line);
		QTimer::singleShot(0, this, SLOT(slotSocketRead()));
	}
}

void Metatlantic::closeSocket(bool doEmitResult)
{
	m_stream.setDevice(nullptr);
	m_socket->close();
	disconnect(m_socket, nullptr, this, nullptr);
	m_socket->deleteLater();
	m_socket = nullptr;
	if (doEmitResult)
		emitResult();
}

void Metatlantic::processMsg(const QString &msg)
{
	qCDebug(ATLANTIK_LOG) << msg;
	QXmlStreamReader reader(msg);
	if (!reader.readNextStartElement() || reader.name() != QLatin1String("meta_atlantic"))
	{
		// Invalid data, close the connection
		closeSocket();
		return;
	}
	bool do_send_follow = false;
	bool do_close = false;
	while (reader.readNextStartElement()) {
		const auto name = reader.name();
		if (name == QLatin1String("metaserver"))
		{
			const auto serverVersion = reader.attributes().value(QStringLiteral("version"));
			if (!serverVersion.isNull())
				qCDebug(ATLANTIK_LOG) << "metaserver version" << serverVersion;
			do_send_follow = true;
		}
		else if (name == QLatin1String("server"))
		{
			const QXmlStreamAttributes attrs = reader.attributes();
			const QString host = attrs.value(QStringLiteral("host")).toString();
			const int port = attrs.value(QStringLiteral("port")).toInt();
			const QString version = attrs.value(QStringLiteral("version")).toString();
			const int users = attrs.value(QStringLiteral("users")).toInt();
			Q_EMIT metatlanticAdd(host, port, version, users);
			do_close = true;
		}
		else
			qCDebug(ATLANTIK_LOG) << "ignored TAG:" << name;
		reader.skipCurrentElement();
	}
	if (do_send_follow) {
		m_stream << "FOLLOW" << '\n';
		m_stream.flush();
	}
	if (do_close)
		closeSocket();
}

#include "moc_metatlantic.cpp"
