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

#ifndef ATLANTIK_SELECTGAME_WIDGET_H
#define ATLANTIK_SELECTGAME_WIDGET_H

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>

#include <klistview.h>
#include <kpushbutton.h>

class SelectGame : public QWidget
{
Q_OBJECT

public:
	SelectGame(QWidget *parent, const char *name=0);

	void initPage();
		bool validateNext();
		QString hostToConnect() const;
		int portToConnect();

	public slots:
		void validateConnectButton();

		void slotGameListClear();
		void slotGameListAdd(QString gameId, QString gameType, QString description, QString players);
		void slotGameListEdit(QString gameId, QString gameType, QString description, QString players);
		void slotGameListDel(QString gameId);

	private slots:
		void connectPressed();

	signals:
		void joinGame(int gameId);
		void newGame(const QString &gameType);
//		void statusChanged();

private:
	QVBoxLayout *m_mainLayout;
	QLabel *status_label;
	KListView *m_gameList;
	KPushButton *m_connectButton;
};

#endif
