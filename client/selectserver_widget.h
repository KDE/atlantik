// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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

#ifndef ATLANTIK_SELECTSERVER_WIDGET_H
#define ATLANTIK_SELECTSERVER_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qlabel.h>

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
		void validateRadioButtons();
		void validateConnectButton();

		void slotMonopigatorAdd(QString host, QString port, QString version, int users);
		void slotListClicked(QListViewItem *);

private slots:
	void slotConnect();
	void customConnect();
	void slotRefresh(bool useMonopigator = true);
	void slotCustomConnected();
	void slotCustomError();
	void monopigatorFinished();
	void monopigatorTimeout();

signals:
	void serverConnect(const QString host, int port);

private:
	void checkCustomServer(const QString &host, int port);
	void initMonopigator();

	QVBoxLayout *m_mainLayout;
	QLabel *status_label;
	QRadioButton *m_localGameButton, *m_onlineGameButton;
	KListView *m_serverList;
	KLineEdit *m_hostEdit, *m_portEdit;
	KPushButton *m_addServerButton, *m_refreshButton, *m_customConnect, *m_connectButton;
	Monopigator *m_monopigator;
	KExtendedSocket *m_localSocket;
	bool m_localServerAvailable, m_hideDevelopmentServers;
};

#endif
