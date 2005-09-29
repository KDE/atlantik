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

#include <qpainter.h>
#include <qstring.h>

#include <kdebug.h>
#include <klocale.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <auction.h>

#include "auction_widget.h"
#include "estatedetails.h"
#include "estateview.h"
#include "token.h"

#include "board.h"
#include "board.moc"

AtlantikBoard::AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, DisplayMode mode, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = atlanticCore;
	m_maxEstates = maxEstates;
	m_mode = mode;
	m_animateTokens = false;
	m_lastServerDisplay = 0;

	setMinimumSize(QSize(500, 500));

	int sideLen = maxEstates/4;

	// Animated token movement
	m_movingToken = 0;
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slotMoveToken()));
	m_resumeTimer = false;

	m_gridLayout = new QGridLayout(this, sideLen+1, sideLen+1);
	for(int i=0;i<=sideLen;i++)
	{
		if (i==0 || i==sideLen)
		{
			m_gridLayout->setRowStretch(i, 3);
			m_gridLayout->setColStretch(i, 3);
		}
		else
		{
			m_gridLayout->setRowStretch(i, 2);
			m_gridLayout->setColStretch(i, 2);
		}
	}

//	spacer = new QWidget(this);
//	m_gridLayout->addWidget(spacer, sideLen, sideLen); // SE

	m_displayQueue.setAutoDelete(true);
	m_estateViews.setAutoDelete(true);
	m_tokens.setAutoDelete(true);

	displayDefault();
}

AtlantikBoard::~AtlantikBoard()
{
	reset();
}

void AtlantikBoard::reset()
{
	kdDebug() << "AtlantikBoard::reset" << endl;

	m_tokens.clear();
	m_estateViews.clear();
	m_displayQueue.clear();
	m_lastServerDisplay = 0;
	m_movingToken = 0;
}

void AtlantikBoard::setViewProperties(bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects, bool animateTokens)
{
	if (m_animateTokens != animateTokens)
		m_animateTokens = animateTokens;

	// Update EstateViews
	EstateView *estateView;
	for (QPtrListIterator<EstateView> it(m_estateViews); *it; ++it)
		if ((estateView = dynamic_cast<EstateView*>(*it)))
			estateView->setViewProperties(indicateUnowned, highliteUnowned, darkenMortgaged, quartzEffects);
}

int AtlantikBoard::heightForWidth(int width)
{
	return width;
}

EstateView *AtlantikBoard::findEstateView(Estate *estate)
{
	EstateView *estateView;
	for (QPtrListIterator<EstateView> i(m_estateViews); *i; ++i)
	{
		estateView = dynamic_cast<EstateView*>(*i);
		if (estateView && estateView->estate() == estate)
			return estateView;
	}
	return 0;
}

void AtlantikBoard::addEstateView(Estate *estate, bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects)
{
	QString icon = QString();
	int estateId = estate->id();
	EstateOrientation orientation = North;
	int sideLen = m_gridLayout->numRows() - 1;

	if (estateId < sideLen)
		orientation = North;
	else if (estateId < 2*sideLen)
		orientation = East;
	else if (estateId < 3*sideLen)
		orientation = South;
	else //if (estateId < 4*sideLen)
		orientation = West;
	
	EstateView *estateView = new EstateView(estate, orientation, icon, indicateUnowned, highliteUnowned, darkenMortgaged, quartzEffects, this, "estateview");
	m_estateViews.append(estateView);

	connect(estate, SIGNAL(changed()), estateView, SLOT(estateChanged()));
	connect(estateView, SIGNAL(estateToggleMortgage(Estate *)), estate, SIGNAL(estateToggleMortgage(Estate *)));
	connect(estateView, SIGNAL(LMBClicked(Estate *)), estate, SIGNAL(LMBClicked(Estate *)));
	connect(estateView, SIGNAL(estateHouseBuy(Estate *)), estate, SIGNAL(estateHouseBuy(Estate *)));
	connect(estateView, SIGNAL(estateHouseSell(Estate *)), estate, SIGNAL(estateHouseSell(Estate *)));
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
		Player *player = 0;
		QPtrList<Player> playerList = m_atlanticCore->players();
		for (QPtrListIterator<Player> it(playerList); (player = *it) ; ++it)
			if (player->location() == estate)
				addToken(player);
	}
}

void AtlantikBoard::addAuctionWidget(Auction *auction)
{
	AuctionWidget *auctionW = new AuctionWidget(m_atlanticCore, auction, this);
	m_lastServerDisplay = auctionW;
	m_displayQueue.prepend(auctionW);
	updateCenter();

	connect(auction, SIGNAL(completed()), this, SLOT(displayDefault()));
}

Token *AtlantikBoard::findToken(Player *player)
{
	Token *token = 0;
	for (QPtrListIterator<Token> it(m_tokens); (token = *it) ; ++it)
		if (token->player() == player)
			return token;
	return 0;
}

void AtlantikBoard::addToken(Player *player)
{
	if (!player->location())
	{
		kdDebug() << "addToken ignored - estateView null" << endl;
		return;
	}

	Player *playerSelf = 0;
	if (m_atlanticCore)
		playerSelf = m_atlanticCore->playerSelf();

	if (playerSelf && playerSelf->game() != player->game() )
	{
		kdDebug() << "addToken ignored - not in same game as playerSelf" << endl;
		return;
	}

	kdDebug() << "addToken" << endl;

	Token *token = new Token(player, this, "token");
	m_tokens.append(token);
	connect(player, SIGNAL(changed(Player *)), token, SLOT(playerChanged()));
	
	jumpToken(token);

	// Timer to reinit the gameboard _after_ event loop
	QTimer::singleShot(100, this, SLOT(slotResizeAftermath()));
}

void AtlantikBoard::playerChanged(Player *player)
{
	kdDebug() << "playerChanged: playerLoc " << (player->location() ? player->location()->name() : "none") << endl;

	Player *playerSelf = 0;
	if (m_atlanticCore)
		playerSelf = m_atlanticCore->playerSelf();

	// Update token
	Token *token = findToken(player);
	if (token)
	{
		kdDebug() << "playerChanged: tokenLoc " << (token->location() ? token->location()->name() : "none") << endl;
		if (player->isBankrupt() || (playerSelf && playerSelf->game() != player->game()) )
			token->hide();
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

	m_tokens.remove(token);
}

void AtlantikBoard::jumpToken(Token *token)
{
	if (!token || !token->location())
		return;

	kdDebug() << "jumpToken to " << token->location()->name() << endl;

	QPoint tGeom = calculateTokenDestination(token);
	token->setGeometry(tGeom.x(), tGeom.y(), token->width(), token->height());

	Player *player = token->player();
	if (player)
	{
		player->setLocation(token->location());
		player->setDestination(0);

		if (token->isHidden() && !player->isBankrupt())
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
	kdDebug() << "moveToken to " << token->destination()->name() << endl;

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
		kdDebug() << "slotMoveToken ignored - no atlanticCore" << endl;
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

//	kdDebug() << "TOKEN: at " << xCurrent << "," << yCurrent << " and going to " << xDest << "," << yDest << endl;

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
	kdDebug() << "AtlantikBoard::slotResizeAftermath" << endl;
	// Move tokens back to their last known location (this has to be done
	// _after_ resizeEvent has returned to make sure we have the correct
	// adjusted estate geometries.

	Token *token = 0;
	for (QPtrListIterator<Token> it(m_tokens); (token = *it) ; ++it)
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
		if (m_displayQueue.getFirst() == m_lastServerDisplay)
			m_lastServerDisplay = 0;
		m_displayQueue.removeFirst();
		break;
	}
	updateCenter();
}

void AtlantikBoard::displayButton(QString command, QString caption, bool enabled)
{
	if (EstateDetails *display = dynamic_cast<EstateDetails*>(m_lastServerDisplay))
		display->addButton(command, caption, enabled);
}

void AtlantikBoard::addCloseButton()
{
	EstateDetails *eDetails = 0;
	if ((eDetails = dynamic_cast<EstateDetails*>(m_lastServerDisplay)) && eDetails != m_displayQueue.getLast())
		eDetails->addCloseButton();
}

void AtlantikBoard::insertDetails(QString text, bool clearText, bool clearButtons, Estate *estate)
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

	if (m_displayQueue.getFirst() != m_lastServerDisplay)
		m_displayQueue.removeFirst();

	eDetails = new EstateDetails(estate, text, this);
	m_lastServerDisplay = eDetails;
	connect(eDetails, SIGNAL(buttonCommand(QString)), this, SIGNAL(buttonCommand(QString)));
	connect(eDetails, SIGNAL(buttonClose()), this, SLOT(displayDefault()));

	m_displayQueue.insert(0, eDetails);
	updateCenter();
}

void AtlantikBoard::prependEstateDetails(Estate *estate)
{
	if (!estate)
		return;

	EstateDetails *eDetails = 0;

	if (m_displayQueue.getFirst() == m_lastServerDisplay)
	{
		eDetails = new EstateDetails(estate, QString::null, this);
		m_displayQueue.prepend(eDetails);

		connect(eDetails, SIGNAL(buttonCommand(QString)), this, SIGNAL(buttonCommand(QString)));
		connect(eDetails, SIGNAL(buttonClose()), this, SLOT(displayDefault()));
	}
	else
	{
		eDetails = dynamic_cast<EstateDetails*> ( m_displayQueue.getFirst() );
		if (eDetails)
		{
			eDetails->setEstate(estate);
			eDetails->setText( QString::null );
			// eDetails->clearButtons();
		}
		else
		{
			kdDebug() << "manual estatedetails with first in queue neither server nor details" << endl;
			return;
		}
	}

	eDetails->addDetails();
	eDetails->addCloseButton();

	updateCenter();
}

void AtlantikBoard::updateCenter()
{
	QWidget *center = m_displayQueue.getFirst();
	m_gridLayout->addMultiCellWidget(center, 1, m_gridLayout->numRows()-2, 1, m_gridLayout->numCols()-2);
	center->show();
}

QWidget *AtlantikBoard::centerWidget()
{
	return m_displayQueue.getFirst();
}
