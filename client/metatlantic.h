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

#include <QObject>
#include <QTextStream>
#include <QAbstractSocket>

class QTcpSocket;

class Metatlantic : public QObject
{
Q_OBJECT

public:
	Metatlantic();
	~Metatlantic();
	void loadData(const QString &host, int port);

signals:
	void metatlanticAdd(const QString &host, int port, const QString &version, int users);
	void finished();
	void timeout();

private slots:
	void slotSocketError(QAbstractSocket::SocketError socketError);
	void slotSocketConnected();
	void slotSocketRead();

private:
	void closeSocket();
	void processMsg(const QString &msg);

	QTcpSocket *m_socket;
	QTextStream m_stream;
};

#endif
