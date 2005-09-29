// Copyright (c) 2002-2003 Rob Kaper <cap@capsi.com>
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
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#ifndef ATLANTIK_BOARD_H
#define ATLANTIK_BOARD_H

#include <qwidget.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qptrlist.h>
#include "libatlantikui_export.h"
class QPoint;

class AtlanticCore;
class Auction;
class Estate;
class Player;
class Token;

class EstateView;

class LIBATLANTIKUI_EXPORT AtlantikBoard : public QWidget
{
Q_OBJECT

public:
	enum DisplayMode { Play, Edit };

	AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, DisplayMode mode, QWidget *parent, const char *name=0);
	~AtlantikBoard();
	void reset();

	void setViewProperties(bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects, bool animateTokens);
	int heightForWidth(int);
	void addEstateView(Estate *estate, bool indicateUnowned = false, bool highliteUnowned = false, bool darkenMortgaged = false, bool quartzEffects = false);
	void addAuctionWidget(Auction *auction);

	void addToken(Player *player);
	void removeToken(Player *player);

	void indicateUnownedChanged();
	EstateView *findEstateView(Estate *estate);
	QWidget *centerWidget();

public slots:
	void slotMoveToken();
	void slotResizeAftermath();
	void displayDefault();

private slots:
	void playerChanged(Player *player);
	void displayButton(QString command, QString caption, bool enabled);
	void prependEstateDetails(Estate *);
	void insertDetails(QString text, bool clearText, bool clearButtons, Estate *estate = 0);
	void addCloseButton();

signals:
	void tokenConfirmation(Estate *estate);
	void buttonCommand(QString command);

protected:
	void resizeEvent(QResizeEvent *);

private:
	Token *findToken(Player *player);
	void jumpToken(Token *token);
	void moveToken(Token *token);
	QPoint calculateTokenDestination(Token *token, Estate *estate = 0);

	void updateCenter();

	AtlanticCore *m_atlanticCore;
	DisplayMode m_mode;

	QWidget *spacer, *m_lastServerDisplay;
	QGridLayout *m_gridLayout;
	Token *m_movingToken;
	QTimer *m_timer;
	bool m_resumeTimer;

	bool m_animateTokens;
	int m_maxEstates;

	QPtrList<EstateView> m_estateViews;
	QPtrList<Token> m_tokens;
	QPtrList<QWidget> m_displayQueue;
};

#endif
