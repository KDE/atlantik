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

#include <klistview.h>
#include <kpushbutton.h>

#include "monopigator.h"

class KExtendedSocket;

class SelectServer : public QWidget
{
Q_OBJECT

public:
	SelectServer(QWidget *parent, const char *name=0);
	virtual ~SelectServer();

	void initPage();
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
		void slotRefresh();
		void slotLocalConnected();
		void slotLocalError();
		void monopigatorFinished();
		void monopigatorTimeout();

	signals:
		void serverConnect(const QString host, int port);
//		void statusChanged();

private:
	void checkLocalServer();
	void initMonopigator();

		QVBoxLayout *m_mainLayout;
		QLabel *status_label;
		QRadioButton *m_localGameButton, *m_onlineGameButton;
		KListView *m_serverList;
		KPushButton *m_refreshButton, *m_connectButton;
		Monopigator *m_monopigator;
		KExtendedSocket *m_localSocket;
		bool m_localServerAvailable;
};

#endif
