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

#ifndef ATLANTIK_METATLANTIC_H
#define ATLANTIK_METATLANTIC_H

#include <kjob.h>

#include <QTextStream>
#include <QAbstractSocket>

class QTcpSocket;

class Metatlantic : public KJob
{
Q_OBJECT

public:
	Metatlantic(const QString &host, int port, QObject *parent = nullptr);
	~Metatlantic();
	void start() override;

Q_SIGNALS:
	void metatlanticAdd(const QString &host, int port, const QString &version, int users);

protected:
	bool doKill() override;

private Q_SLOTS:
	void slotSocketError(QAbstractSocket::SocketError socketError);
	void slotSocketConnected();
	void slotSocketRead();

private:
	void closeSocket(bool doEmitResult = true);
	void processMsg(const QString &msg);

	QString m_host;
	int m_port;
	QTcpSocket *m_socket;
	QTextStream m_stream;
};

#endif
