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

#include <QWidget>
#include <QTimer>
#include <QList>
#include <QHash>

#include "libatlantikui_export.h"
#include "tokentheme.h"

class QGridLayout;
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

	AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, DisplayMode mode, QWidget *parent);
	~AtlantikBoard();
	void reset();

	void setViewProperties(bool indicateUnowned, bool highlightUnowned, bool darkenMortgaged, bool quartzEffects, bool animateTokens);
	int heightForWidth(int) const override;
	void addEstateView(Estate *estate, bool indicateUnowned = false, bool highlightUnowned = false, bool darkenMortgaged = false, bool quartzEffects = false);
	void addAuctionWidget(Auction *auction);

	void addToken(Player *player);
	void removeToken(Player *player);

	EstateView *findEstateView(Estate *estate) const;
	QWidget *centerWidget();

	void setTokenTheme(const TokenTheme &theme);

public Q_SLOTS:
	void slotMoveToken();
	void slotResizeAftermath();
	void displayDefault();

private Q_SLOTS:
	void playerChanged(Player *player);
	void displayButton(const QString &command, const QString &caption, bool enabled);
	void prependEstateDetails(Estate *);
	void insertDetails(const QString &text, bool clearText, bool clearButtons, Estate *estate = nullptr);
	void insertText(const QString &text, bool clearText, bool clearButtons);
	void addCloseButton();

Q_SIGNALS:
	void tokenConfirmation(Estate *estate);
	void buttonCommand(const QString &command);

protected:
	void resizeEvent(QResizeEvent *) override;

private:
	Token *findToken(Player *player) const;
	void jumpToken(Token *token);
	void moveToken(Token *token);
	QPoint calculateTokenDestination(Token *token, Estate *estate = nullptr);

	void updateCenter();

	AtlanticCore *m_atlanticCore;
	DisplayMode m_mode;

	QWidget *spacer, *m_lastServerDisplay, *m_lastServerDisplayBeforeAuction;
	QGridLayout *m_gridLayout;
	Token *m_movingToken;
	QTimer *m_timer;
	bool m_resumeTimer;

	bool m_animateTokens;
	int m_maxEstates;

	QHash<Estate *, EstateView *> m_estateViews;
	QHash<Player *, Token *> m_tokens;
	QList<QWidget *> m_displayQueue;

	TokenTheme m_tokenTheme;
};

#endif
