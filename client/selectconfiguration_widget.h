// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
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

#ifndef ATLANTIK_SELECTCONFIGURATION_WIDGET_H
#define ATLANTIK_SELECTCONFIGURATION_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qlabel.h>

#include <klistview.h>
#include <kpushbutton.h>

class SelectConfiguration : public QWidget
{
Q_OBJECT

public:
	SelectConfiguration(QWidget *parent, const char *name=0);

	void initPage();
		bool validateNext();
		QString hostToConnect() const;
		int portToConnect();

	public slots:
		void slotPlayerListClear();
		void slotPlayerListAdd(QString playerId, QString name, QString host);
		void slotPlayerListEdit(QString playerId, QString name, QString host);
		void slotPlayerListDel(QString playerId);

private slots:
	void connectPressed();
	void slotClicked();
	void gameOption(QString title, QString type, QString value);

signals:
	void startGame();
	void leaveGame();
	void joinConfiguration(int configurationId);
	void newConfiguration();
//	void statusChanged();

	private:
		QVBoxLayout *m_mainLayout;
		QLabel *status_label;
		QVGroupBox *m_playerBox, *m_configBox, *m_messageBox;
		KListView *m_playerList;
		KPushButton *m_backButton, *m_connectButton;
};

#endif
