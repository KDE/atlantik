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

class QTcpSocket;
class QTreeWidget;

class KLineEdit;
class KPushButton;

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

	bool operator<(const QTreeWidgetItem &other) const;

private slots:
	void resolved();
	void connected();
	void showDevelopmentServers(bool show);

private:
	int m_latency;
	int m_users;
	int m_port;
	QTcpSocket *m_latencySocket;
	QElapsedTimer m_timer;
	bool m_isDev;
};

class SelectServer : public QWidget
{
Q_OBJECT

public:
	SelectServer(bool useMonopigatorOnStart, bool hideDevelopmentServers, QWidget *parent);
	virtual ~SelectServer();

	void setHideDevelopmentServers(bool hideDevelopmentServers);

public slots:
	void validateConnectButton();
	void slotRefresh(bool useMonopigator = true);
	void slotMetatlanticAdd(const QString &host, int port, const QString &version, int users);

private slots:
	void slotConnect();
	void customConnect();
	void metatlanticFinished();
	void metatlanticTimeout();

signals:
	void serverConnect(const QString &host, int port);
	void msgStatus(const QString &message);
	void showDevelopmentServers(bool show);

private:
	void initMonopigator();

	QTreeWidget *m_serverList;
	KLineEdit *m_hostEdit, *m_portEdit;
	KPushButton *m_refreshButton, *m_connectButton;
	Metatlantic *m_metatlantic;
	bool m_hideDevelopmentServers;
};

#endif
