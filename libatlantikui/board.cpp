// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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

#include <iostream>

#include <QGridLayout>
#include <QResizeEvent>

#include <klocalizedstring.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <auction.h>

#include "auction_widget.h"
#include "configoption.h"
#include "estatedetails.h"
#include "estateview.h"
#include "token.h"

#include "board.h"

#include <libatlantikui_debug.h>

AtlantikBoard::AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, DisplayMode mode, QWidget *parent) : QWidget(parent)
{
	m_atlanticCore = atlanticCore;
	m_maxEstates = maxEstates;
	m_mode = mode;
	m_animateTokens = false;
	m_lastServerDisplay = 0;
	m_lastServerDisplayBeforeAuction = 0;
	m_allowestatesalesOption = 0;

	setMinimumSize(QSize(500, 500));

	int sideLen = maxEstates/4;

	// Animated token movement
	m_movingToken = 0;
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slotMoveToken()));
	m_resumeTimer = false;

	m_gridLayout = new QGridLayout( this );
	m_gridLayout->setSpacing(0);
	m_gridLayout->setMargin(0);
	for(int i=0;i<=sideLen;i++)
	{
		if (i==0 || i==sideLen)
		{
			m_gridLayout->setRowStretch(i, 3);
			m_gridLayout->setColumnStretch(i, 3);
		}
		else
		{
			m_gridLayout->setRowStretch(i, 2);
			m_gridLayout->setColumnStretch(i, 2);
		}
	}

//	spacer = new QWidget(this);
//	m_gridLayout->addWidget(spacer, sideLen, sideLen); // SE

	displayDefault();

	connect(m_atlanticCore, SIGNAL(createGUI(ConfigOption *)), this, SLOT(slotConfigOptionAdded(ConfigOption *)));
	connect(m_atlanticCore, SIGNAL(removeGUI(ConfigOption *)), this, SLOT(slotConfigOptionRemoved(ConfigOption *)));

	if (m_atlanticCore)
	{
		if (ConfigOption *opt = m_atlanticCore->findConfigOption("allowestatesales"))
			slotConfigOptionAdded(opt);
	}
}

AtlantikBoard::~AtlantikBoard()
{
	reset();
}

void AtlantikBoard::reset()
{
	qCDebug(LIBATLANTIKUI_LOG);

	qDeleteAll(m_tokens);
	m_tokens.clear();
	qDeleteAll(m_estateViews);
	m_estateViews.clear();
	qDeleteAll(m_displayQueue);
	m_displayQueue.clear();
	m_lastServerDisplay = 0;
	m_movingToken = 0;
}

void AtlantikBoard::setViewProperties(bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects, bool animateTokens)
{
	if (m_animateTokens != animateTokens)
		m_animateTokens = animateTokens;

	// Update EstateViews
	foreach (EstateView *estateView, m_estateViews)
			estateView->setViewProperties(indicateUnowned, highliteUnowned, darkenMortgaged, quartzEffects);
}

int AtlantikBoard::heightForWidth(int width) const
{
	return width;
}

EstateView *AtlantikBoard::findEstateView(Estate *estate) const
{
	foreach (EstateView *estateView, m_estateViews)
	{
		if (estateView->estate() == estate)
			return estateView;
	}
	return 0;
}

void AtlantikBoard::addEstateView(Estate *estate, bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects)
{
	QString icon = estate->icon();
	int estateId = estate->id();
	EstateOrientation orientation = North;
	int sideLen = m_gridLayout->rowCount() - 1;

	if (estateId < sideLen)
		orientation = North;
	else if (estateId < 2*sideLen)
		orientation = East;
	else if (estateId < 3*sideLen)
		orientation = South;
	else //if (estateId < 4*sideLen)
		orientation = West;

	EstateView *estateView = new EstateView(estate, orientation, icon, indicateUnowned, highliteUnowned, darkenMortgaged, quartzEffects, this);
        estateView->setObjectName( "estateview" );
	if (m_allowestatesalesOption)
		estateView->setAllowEstateSales(m_allowestatesalesOption->value().toInt() != 0);
	m_estateViews.append(estateView);

	connect(estate, SIGNAL(changed()), estateView, SLOT(estateChanged()));
	connect(estateView, SIGNAL(estateToggleMortgage(Estate *)), estate, SIGNAL(estateToggleMortgage(Estate *)));
	connect(estateView, SIGNAL(LMBClicked(Estate *)), estate, SIGNAL(LMBClicked(Estate *)));
	connect(estateView, SIGNAL(estateHouseBuy(Estate *)), estate, SIGNAL(estateHouseBuy(Estate *)));
	connect(estateView, SIGNAL(estateHouseSell(Estate *)), estate, SIGNAL(estateHouseSell(Estate *)));
	connect(estateView, SIGNAL(estateSell(Estate *)), estate, SIGNAL(estateSell(Estate *)));
	connect(estateView, SIGNAL(newTrade(Player *)), estate, SIGNAL(newTrade(Player *)));

	// Designer has its own LMBClicked slot
	if (m_mode == Play)
		connect(estateView, SIGNAL(LMBClicked(Estate *)), this, SLOT(prependEstateDetails(Estate *)));

	if (estateId<sideLen)
		m_gridLayout->addWidget(estateView, sideLen, sideLen-estateId);
	else if (estateId<2*sideLen)
		m_gridLayout->addWidget(estateView, 2*sideLen-estateId, 0);
	else if (estateId<3*sideLen)
		m_gridLayout->addWidget(estateView, 0, estateId-2*sideLen);
	else
		m_gridLayout->addWidget(estateView, estateId-3*sideLen, sideLen);

	estateView->show();

	if (m_atlanticCore)
	{
		foreach (Player *player, m_atlanticCore->players())
			if (player->location() == estate)
				addToken(player);
	}
}

void AtlantikBoard::addAuctionWidget(Auction *auction)
{
	AuctionWidget *auctionW = new AuctionWidget(m_atlanticCore, auction, this);
	m_lastServerDisplayBeforeAuction = m_lastServerDisplay;
	m_lastServerDisplay = auctionW;
	m_displayQueue.prepend(auctionW);
	updateCenter();

	connect(auction, SIGNAL(completed()), this, SLOT(displayDefault()));
}

Token *AtlantikBoard::findToken(Player *player) const
{
	foreach (Token *token, m_tokens)
		if (token->player() == player)
			return token;
	return 0;
}

void AtlantikBoard::addToken(Player *player)
{
	if (!player->location())
	{
		qCDebug(LIBATLANTIKUI_LOG) << "addToken ignored - estateView null";
		return;
	}

	Player *playerSelf = 0;
	if (m_atlanticCore)
		playerSelf = m_atlanticCore->playerSelf();

	if (playerSelf && playerSelf->game() != player->game() )
	{
		qCDebug(LIBATLANTIKUI_LOG) << "addToken ignored - not in same game as playerSelf";
		return;
	}

	qCDebug(LIBATLANTIKUI_LOG) << "addToken";

	Token *token = new Token(player, this);
        token->setObjectName( "token" );
	token->setTokenTheme(m_tokenTheme);
	m_tokens.append(token);
	connect(player, SIGNAL(changed(Player *)), token, SLOT(playerChanged()));

	jumpToken(token);

	// Timer to reinit the gameboard _after_ event loop
	QTimer::singleShot(100, this, SLOT(slotResizeAftermath()));
}

void AtlantikBoard::playerChanged(Player *player)
{
	qCDebug(LIBATLANTIKUI_LOG) << "playerLoc" << (player->location() ? player->location()->name() : "none");

	Player *playerSelf = 0;
	if (m_atlanticCore)
		playerSelf = m_atlanticCore->playerSelf();

	// Update token
	Token *token = findToken(player);
	if (token)
	{
		qCDebug(LIBATLANTIKUI_LOG) << "tokenLoc" << (token->location() ? token->location()->name() : "none");
		if (player->isBankrupt() || player->isSpectator() || (playerSelf && playerSelf->game() != player->game()) )
			token->hide();
		if (!player->isSpectator())
			token->show();
		if (player->hasTurn())
			token->raise();

		bool jump = false, move = false;

		if (token->inJail() != player->inJail())
		{
			token->setInJail(player->inJail());

			// If any status such as jail is ever allowed to
			// change in the future, during movement, this needs
			// to be addressed in moveToken and subsequent steps.
			if (token != m_movingToken)
				jump = true;
		}

		if (token->location() != player->location())
		{
			token->setLocation(player->location());
			jump = true;
		}

		if (player->destination() && token->destination() != player->destination())
		{
			if (m_animateTokens)
			{
				token->setDestination(player->destination());
				move = true;
			}
			else
			{
				token->setLocation(player->destination());
				jump = true;
			}
		}

		if (move)
			moveToken(token);
		else if (jump)
			jumpToken(token);
	}
	else
		addToken(player);
}

void AtlantikBoard::removeToken(Player *player)
{
	Token *token = findToken(player);
	if (!token)
		return;

	if (token == m_movingToken)
	{
		m_timer->stop();
		m_movingToken = 0;
	}

	m_tokens.removeOne(token);
	delete token;
}

void AtlantikBoard::jumpToken(Token *token)
{
	if (!token || !token->location())
		return;

	qCDebug(LIBATLANTIKUI_LOG) << "to" << token->location()->name();

	QPoint tGeom = calculateTokenDestination(token);
	token->setGeometry(tGeom.x(), tGeom.y(), token->width(), token->height());

	Player *player = token->player();
	if (player)
	{
		player->setLocation(token->location());
		player->setDestination(0);

		if (token->isHidden() && !player->isBankrupt() && !player->isSpectator())
			token->show();
	}

	if (token == m_movingToken)
	{
		m_timer->stop();

		if (!m_resumeTimer)
			m_movingToken = 0;
	}

	emit tokenConfirmation(token->location());
}

void AtlantikBoard::moveToken(Token *token)
{
	qCDebug(LIBATLANTIKUI_LOG) << "to" << token->destination()->name();

	m_movingToken = token;

	// Start timer
	m_timer->start(15);
}

QPoint AtlantikBoard::calculateTokenDestination(Token *token, Estate *eDest)
{
		if (!eDest)
			eDest = token->location();

		EstateView *evDest = findEstateView(eDest);
		if (!evDest)
			return QPoint(0, 0);

		int x = 0, y = 0;
		if (token->player()->inJail())
		{
			x = evDest->geometry().right() - token->width() - 2;
			y = evDest->geometry().top();
		}
		else
		{
			x = evDest->geometry().center().x() - (token->width()/2);
			y = evDest->geometry().center().y() - (token->height()/2);

/*
			// Re-center because of EstateView headers
			switch(evDest->orientation())
			{
				case North:
					y += evDest->height()/8; break;
				case East:
					x -= evDest->width()/8; break;
				case South:
					y -= evDest->height()/8; break;
				case West:
					x += evDest->width()/8; break;
			}
*/
		}
	return QPoint(x, y);
}

void AtlantikBoard::slotMoveToken()
{
	// Requires a core with estates to operate on
	if (!m_atlanticCore)
	{
		qCDebug(LIBATLANTIKUI_LOG) << "ignored - no atlanticCore";
		return;
	}

	// Do we actually have a token to move?
	if (!m_movingToken)
	{
		m_timer->stop();
		return;
	}

	// Where are we?
	int xCurrent = m_movingToken->geometry().x();
	int yCurrent = m_movingToken->geometry().y();

	// Where do we want to go today?
	Estate *eDest = m_atlanticCore->estateAfter(m_movingToken->location());
	QPoint tGeom = calculateTokenDestination(m_movingToken, eDest);

	int xDest = tGeom.x();
	int yDest = tGeom.y();

	if (xDest - xCurrent > 1)
		xDest = xCurrent + 2;
	else if (xCurrent - xDest > 1)
		xDest = xCurrent - 2;
	else
		xDest = xCurrent;

	if (yDest - yCurrent > 1)
		yDest = yCurrent + 2;
	else if (yCurrent - yDest > 1)
		yDest = yCurrent - 2;
	else
		yDest = yCurrent;

//	qCDebug(LIBATLANTIKUI_LOG) << "TOKEN: at " << xCurrent << "," << yCurrent << " and going to " << xDest << "," << yDest;

	if (xCurrent != xDest || yCurrent != yDest)
	{
		m_movingToken->setGeometry(xDest, yDest, m_movingToken->width(), m_movingToken->height());
		return;
	}

	// We have arrived at our destination!
	m_movingToken->setLocation(eDest);
	m_movingToken->player()->setLocation(eDest);
	emit tokenConfirmation(eDest);

	// We have arrived at our _final_ destination!
	if (eDest == m_movingToken->destination())
	{
		m_movingToken->setDestination(0);
		m_movingToken->player()->setDestination(0);

		m_timer->stop();
		m_movingToken = 0;
	}
}

void AtlantikBoard::resizeEvent(QResizeEvent *)
{
	// Stop moving tokens, slotResizeAftermath will re-enable this
	if (m_timer!=0 && m_timer->isActive())
	{
		m_timer->stop();
		m_resumeTimer=true;
	}

/*
	// Adjust spacer to make sure board stays a square
	int q = e->size().width() - e->size().height();
	if (q > 0)
	{
		QSize s(q, 0);
		spacer->setFixedSize(s);
	}
	else
	{
		QSize s(0, -q);
		spacer->setFixedSize(s);
	}
*/
	// Timer to reinit the gameboard _after_ resizeEvent
	QTimer::singleShot(0, this, SLOT(slotResizeAftermath()));
}

void AtlantikBoard::slotResizeAftermath()
{
	qCDebug(LIBATLANTIKUI_LOG);
	// Move tokens back to their last known location (this has to be done
	// _after_ resizeEvent has returned to make sure we have the correct
	// adjusted estate geometries.

	foreach (Token *token, m_tokens)
		jumpToken(token);

	// Restart the timer that was stopped in resizeEvent
	if (m_resumeTimer && m_timer!=0 && !m_timer->isActive())
	{
		m_timer->start(15);
		m_resumeTimer=false;
	}
}

void AtlantikBoard::displayDefault()
{
	switch(m_displayQueue.count())
	{
	case 0:
		m_displayQueue.prepend(new QWidget(this));
		break;
	case 1:
		if (EstateDetails *display = dynamic_cast<EstateDetails*>(m_lastServerDisplay))
			display->setEstate(0);
		break;
	default:
		if (m_displayQueue.first() == m_lastServerDisplay)
			m_lastServerDisplay = 0;
		delete m_displayQueue.takeFirst();
		if (m_lastServerDisplayBeforeAuction)
			m_lastServerDisplay = m_lastServerDisplayBeforeAuction;
		break;
	}
	updateCenter();
}

void AtlantikBoard::displayButton(const QString &command, const QString &caption, bool enabled)
{
	if (EstateDetails *display = dynamic_cast<EstateDetails*>(m_lastServerDisplay))
		display->addButton(command, caption, enabled);
}

void AtlantikBoard::addCloseButton()
{
	EstateDetails *eDetails = 0;
	if ((eDetails = dynamic_cast<EstateDetails*>(m_lastServerDisplay)) && eDetails != m_displayQueue.last())
		eDetails->addCloseButton();
}

void AtlantikBoard::insertDetails(const QString &text, bool clearText, bool clearButtons, Estate *estate)
{
	EstateDetails *eDetails = 0;

	if ((eDetails = dynamic_cast<EstateDetails*>(m_lastServerDisplay)))
	{
		if (clearText)
			eDetails->setText(text);
		else
			eDetails->appendText(text);

		if (clearButtons)
			eDetails->clearButtons();

		eDetails->setEstate(estate);
		return;
	}

	if (!m_displayQueue.isEmpty() && m_displayQueue.first() != m_lastServerDisplay)
		delete m_displayQueue.takeFirst();

	eDetails = new EstateDetails(estate, text, this);
	m_lastServerDisplay = eDetails;
	connect(eDetails, SIGNAL(buttonCommand(QString)), this, SIGNAL(buttonCommand(QString)));
	connect(eDetails, SIGNAL(buttonClose()), this, SLOT(displayDefault()));

	m_displayQueue.insert(0, eDetails);
	updateCenter();
}

void AtlantikBoard::insertText(const QString &text, bool clearText, bool clearButtons)
{
	EstateDetails *eDetails = dynamic_cast<EstateDetails*>(m_lastServerDisplay);
	if (!eDetails)
		return;

	if (clearText)
		eDetails->setText(text);
	else
		eDetails->appendText(text);

	if (clearButtons)
		eDetails->clearButtons();
}

void AtlantikBoard::prependEstateDetails(Estate *estate)
{
	if (!estate)
		return;

	EstateDetails *eDetails = 0;

	if (m_displayQueue.first() == m_lastServerDisplay)
	{
		eDetails = new EstateDetails(estate, QString(), this);
		m_displayQueue.prepend(eDetails);

		connect(eDetails, SIGNAL(buttonCommand(QString)), this, SIGNAL(buttonCommand(QString)));
		connect(eDetails, SIGNAL(buttonClose()), this, SLOT(displayDefault()));
	}
	else
	{
		eDetails = dynamic_cast<EstateDetails*>(m_displayQueue.first());
		if (eDetails)
		{
			eDetails->setEstate(estate);
			eDetails->setText( QString() );
			// eDetails->clearButtons();
		}
		else
		{
			qCDebug(LIBATLANTIKUI_LOG) << "manual estatedetails with first in queue neither server nor details";
			return;
		}
	}

	eDetails->addDetails();
	eDetails->addCloseButton();

	updateCenter();
}

void AtlantikBoard::updateCenter()
{
	QWidget *center = m_displayQueue.first();
	m_gridLayout->addWidget(center, 1, 1, m_gridLayout->rowCount()-2, m_gridLayout->columnCount()-2);
	center->show();
}

QWidget *AtlantikBoard::centerWidget()
{
	return m_displayQueue.first();
}

void AtlantikBoard::setTokenTheme(const TokenTheme &theme)
{
	m_tokenTheme = theme;
	foreach (Token *token, m_tokens)
		token->setTokenTheme(m_tokenTheme);
}

void AtlantikBoard::slotConfigOptionAdded(ConfigOption *opt)
{
	if (m_allowestatesalesOption)
		return;

	connect(opt, SIGNAL(changed(ConfigOption *)), this, SLOT(slotConfigOptionChanged(ConfigOption *)));
	if (opt->name() != "allowestatesales")
		return;

	m_allowestatesalesOption = opt;
}

void AtlantikBoard::slotConfigOptionChanged(ConfigOption *opt)
{
	if (opt->name() != "allowestatesales")
		return;

	m_allowestatesalesOption = opt;
	const bool value = m_allowestatesalesOption->value().toInt() != 0;
	foreach (EstateView *view, m_estateViews)
		view->setAllowEstateSales(value);
}

void AtlantikBoard::slotConfigOptionRemoved(ConfigOption *opt)
{
	if (!m_allowestatesalesOption || opt->name() != m_allowestatesalesOption->name())
		return;

	disconnect(m_allowestatesalesOption, 0, this, 0);
	m_allowestatesalesOption = 0;
}
