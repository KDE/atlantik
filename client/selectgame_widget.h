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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_SELECTGAME_WIDGET_H
#define ATLANTIK_SELECTGAME_WIDGET_H

#include <QWidget>

#include <ui_selectgame.h>

class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

class AtlanticCore;
class Game;
class Player;

class SelectGame : public QWidget, private Ui::SelectGame
{
Q_OBJECT

public:
	SelectGame(AtlanticCore *atlanticCore, QWidget *parent);

private Q_SLOTS:
	void connectClicked();
	void addGame(Game *game);
	void delGame(Game *game);
	void updateGame(Game *game);
	void playerChanged(Player *player);
	void validateConnectButton();

Q_SIGNALS:
	void joinGame(int gameId);
	void watchGame(int gameId);
	void newGame(const QString &gameType);
	void leaveServer();
	void msgStatus(const QString &status);

private:
	QTreeWidgetItem *findItem(Game *game);

	AtlanticCore *m_atlanticCore;
};

#endif
