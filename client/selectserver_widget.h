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

#ifndef ATLANTIK_SELECTSERVER_WIDGET_H
#define ATLANTIK_SELECTSERVER_WIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QElapsedTimer>
#include <QVector>

#include <ui_selectserver.h>

class QTcpSocket;

class Metatlantic;

class MetaserverEntry : public QObject, public QTreeWidgetItem
{
Q_OBJECT

public:
	enum { MetaserverType = UserType + 1 };

	MetaserverEntry(const QString &host, int port, const QString &version, int users);
	bool isDev() const;
	QString host() const;
	int port() const;

	bool operator<(const QTreeWidgetItem &other) const override;

private Q_SLOTS:
	void resolved();
	void connected();

private:
	int m_latency;
	int m_users;
	int m_port;
	QTcpSocket *m_latencySocket;
	QElapsedTimer m_timer;
	bool m_isDev;
};

class SelectServer : public QWidget, private Ui::SelectServer
{
Q_OBJECT

public:
	SelectServer(bool hideDevelopmentServers, QWidget *parent);
	virtual ~SelectServer();

	void setHideDevelopmentServers(bool hideDevelopmentServers);

public Q_SLOTS:
	void validateConnectButton();
	void validateCustomConnectButton();
	void reloadServerList();
	void slotMetatlanticAdd(const QString &host, int port, const QString &version, int users);

private Q_SLOTS:
	void slotConnect();
	void customConnect();
	void metatlanticFinished();

Q_SIGNALS:
	void serverConnect(const QString &host, int port);
	void msgStatus(const QString &message);

private:
	void initMetaserver();

	Metatlantic *m_metatlantic;
	bool m_hideDevelopmentServers;
	QVector<MetaserverEntry *> m_developmentServers;
};

#endif
