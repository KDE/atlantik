// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qptrlist.h>

#include "token.h"

class AtlanticCore;
class Auction;
class Estate;
class Player;

class EstateView;

class AtlantikBoard : public QWidget
{
Q_OBJECT

public:
	enum DisplayMode { Play, Edit };

	AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, DisplayMode mode, QWidget *parent, const char *name=0);
	void setViewProperties(bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects, bool animateTokens);
	int heightForWidth(int);
	void addEstateView(Estate *estate, bool indicateUnowned = false, bool highliteUnowned = false, bool darkenMortgaged = false, bool quartzEffects = false);
	void addAuctionWidget(Auction *auction);
	void addToken(Player *player, EstateView *location = 0);
	void indicateUnownedChanged();
	QWidget *centerWidget() { return m_center; };
	QPtrList<EstateView> estateViews();

public slots:
	void slotMoveToken();
	void slotResizeAftermath();

private slots:
	void playerChanged();
	void displayDefault();
	void displayText(QString caption, QString body);
	void displayButton(QString command, QString caption, bool enabled);
	void displayEstateDetails(Estate *);

signals:
	void tokenConfirmation(Estate *estate);
	void buttonCommand(QString command);

protected:
	void resizeEvent(QResizeEvent *);
	QWidget *m_center;

private:
	AtlanticCore *m_atlanticCore;
	DisplayMode m_mode;

	EstateView *getEstateView(Estate *estate);
	void moveToken(Token *, int destination);

	QWidget *spacer;
	QGridLayout *m_gridLayout;
	Token *move_token;
	QTimer *m_timer;
	bool m_resumeTimer;

	bool m_animateTokens;
	int m_maxEstates;

	QPtrList<EstateView> m_estateViews;
	QMap<Player *, Token *> tokenMap;
};

#endif
