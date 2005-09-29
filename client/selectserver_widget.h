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

#include <qwidget.h>
#include <qlayout.h>
#include <qradiobutton.h>

#include <klineedit.h>
#include <klistview.h>
#include <kpushbutton.h>

#include "monopigator.h"

class KExtendedSocket;

class SelectServer : public QWidget
{
Q_OBJECT

public:
	SelectServer(bool useMonopigatorOnStart, bool hideDevelopmentServers, QWidget *parent, const char *name=0);
	virtual ~SelectServer();

	void initPage();
	void setHideDevelopmentServers(bool hideDevelopmentServers);
	bool validateNext();
	QString hostToConnect() const;
	int portToConnect();

public slots:
	void validateConnectButton();
	void slotRefresh(bool useMonopigator = true);
	void slotMonopigatorAdd(QString ip, QString host, QString port, QString version, int users);

private slots:
	void slotConnect();
	void customConnect();
	void monopigatorFinished();
	void monopigatorTimeout();

signals:
	void serverConnect(const QString host, int port);
	void msgStatus(const QString &message);
	void showDevelopmentServers(bool show);

private:
	void initMonopigator();

	QVBoxLayout *m_mainLayout;
	KListView *m_serverList;
	KLineEdit *m_hostEdit, *m_portEdit;
	KPushButton *m_addServerButton, *m_refreshButton, *m_customConnect, *m_connectButton;
	Monopigator *m_monopigator;
	bool m_hideDevelopmentServers;
};

#endif
