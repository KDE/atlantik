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

#include <iostream>

#include <qpainter.h>

#include <kdebug.h>
#include <klocale.h>

#include <atlantic_core.h>
#include <player.h>
#include <estate.h>
#include <auction.h>

#include "auction_widget.h"
#include "display_widget.h"
#include "estatedetails.h"
#include "estateview.h"

#include "board.h"
#include "board.moc"

AtlantikBoard::AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, DisplayMode mode, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = atlanticCore;
	m_maxEstates = maxEstates;
	m_mode = mode;
	m_animateTokens = false;
	m_lastServerDisplay = 0;

	int sideLen = maxEstates/4;

	// Timer for token movement
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
	displayDefault();

	QColor color;
	QString icon;
	bool canBeOwned;

	for (int i=0;i<maxEstates;i++)
	{
		color = QColor();
		icon = QString();
		canBeOwned = false;

		switch(i)
		{
			case 5: case 15: case 25: case 35:
				icon = QString("train.png");
				canBeOwned = true;
				break;
			case 12:
				icon = QString("energy.png");
				canBeOwned = true;
				break;
			case 28:
				icon = QString("water.png");
				canBeOwned = true;
				break;
			case 2: case 17: case 33:
				icon = QString("box.png");
				break;
			case 7: case 36:
				icon = QString("qmark-red.png");
				break;
			case 22:
				icon = QString("qmark-blue.png");
				break;
			case 0:
				icon = QString("arrow.png");
				break;
			case 38:
				icon = QString("ring.png");
				break;
		}
	}
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

QPtrList<EstateView> AtlantikBoard::estateViews()
{
	return m_estateViews;
}


EstateView *AtlantikBoard::getEstateView(Estate *estate)
{
	EstateView *estateView;
	for (QPtrListIterator<EstateView> i(m_estateViews); *i; ++i)
	{
		estateView = dynamic_cast<EstateView*>(*i);
		if (estateView->estate() == estate)
			return estateView;
	}
	return 0;
}

void AtlantikBoard::addEstateView(Estate *estate, bool indicateUnowned, bool highliteUnowned, bool darkenMortgaged, bool quartzEffects)
{
	QString icon = QString();
	int estateId = estate->estateId();
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

void AtlantikBoard::addToken(Player *player, EstateView *estateView)
{
	EstateView *evTmp = 0;
	if (!estateView)
	{
		for (QPtrListIterator<EstateView> it(m_estateViews); (evTmp = *it) ; ++it)
			if (evTmp->estate() == player->location())
			{
				estateView = evTmp;
				break;
			}
	}

	if (!estateView)
		return;

	Token *token = new Token(player, estateView, this, "token");
	tokenMap[player] = token;
	connect(player, SIGNAL(changed(Player *)), token, SLOT(playerChanged()));

	// Timer to reinit the gameboard _after_ event loop
	QTimer::singleShot(100, this, SLOT(slotResizeAftermath()));
}

void AtlantikBoard::playerChanged()
{
	kdDebug() << "Board::playerChanged()" << endl;
#warning implement decent AtlantikBoard::playerChanged to update tokens
/*
	kdDebug() << "new geometry for token: " << m_player->location() << endl;
	this->show();
//	setGeometry(100, 100, 125, 125);

	EstateView *estateView = estateViewMap[estateId];
	Token *token = tokenMap[player];
	
	if (estateView && token)
	{
		// Only take action when location has changed
		if (token->location() != estateId)
		{
			if (directMove)
				token->setLocation(estateView, false);
			else if (m_animateTokens==false)
				token->setLocation(estateView);
			else
				moveToken(token, estateId);
		}
	}
*/
}

void AtlantikBoard::moveToken(Token *token, int estateId)
{
	if ( token==0 )
		return;
	
	kdDebug() << "moving piece from " << token->location() << " to " << estateId << endl;

	// Set token destination
	move_token = token;
#warning port
//	move_token->setDestination(estateId);

	// Start timer
	m_timer->start(15);
}

void AtlantikBoard::slotMoveToken()
{
	// Do we actually have a token to move?
	if (move_token==0)
	{
		m_timer->stop();
		return;
	}

	// Where are we?
	int xCurrent = move_token->geometry().x();
	int yCurrent = move_token->geometry().y();

	// Where do we want to go today?
#warning port
	int dest = 0; // move_token->location() + 1;
	if (dest==(m_gridLayout->numRows() - 1)*4)
		dest = 0;
	kdDebug() << "going from " << move_token->location() << " to " << dest << endl;

#warning port
/*
	if (EstateView *estateView = getEstateView(dest))
	{
		int xFinal = estateView->geometry().center().x() - (move_token->width()/2);
		int yFinal = estateView->geometry().center().y() - (move_token->height()/2);
		int xDest, yDest;

		if (xFinal - xCurrent > 1)
			xDest = xCurrent + 2;
		else if (xCurrent - xFinal > 1)
			xDest = xCurrent - 2;
		else
			xDest = xCurrent;

		if (yFinal - yCurrent > 1)
			yDest = yCurrent + 2;
		else if (yCurrent - yFinal > 1)
			yDest = yCurrent - 2;
		else
			yDest = yCurrent;

		kdDebug() << "TOKEN: we are @ " << xCurrent << "," << yCurrent << endl;
		kdDebug() << "TOKEN: final to " << xFinal << "," << yFinal << endl;
		kdDebug() << "TOKEN: going to " << xDest << "," << yDest << endl;

		if (xCurrent == xDest && yCurrent == yDest)
		{
			// We have arrived at our destination!
			move_token->setLocation(estateView);

			// We need to confirm passing Go and arriving at our final
			// destination to the server.
			if (move_token->destination() == move_token->location())
			{
				// We have arrived at our _final_ destination!
#warning port
//				emit tokenConfirmation(move_token->location());
				m_timer->stop();
				move_token = 0;
			}
#warning port
//			else if (move_token->location() == 0)
//				emit tokenConfirmation(move_token->location());

			return;
		}
		
		move_token->setGeometry(xDest, yDest, move_token->width(), move_token->height());
	}
*/
}

void AtlantikBoard::resizeEvent(QResizeEvent *e)
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
	for (QMap<Player *, Token *>::Iterator it=tokenMap.begin() ; it != tokenMap.end() && (token = *it) ; ++it)
		token->updateGeometry();

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
		if (m_displayQueue.getFirst() == m_lastServerDisplay)
			m_lastServerDisplay = 0;
		m_displayQueue.removeFirst();
		m_displayQueue.prepend(new QWidget(this));
		break;
	default:
		if (m_displayQueue.getFirst() == m_lastServerDisplay)
			m_lastServerDisplay = 0;
		m_displayQueue.removeFirst();
		break;
	}
	updateCenter();
}

void AtlantikBoard::displayText(QString caption, QString body)
{
	BoardDisplay *bDisplay = new BoardDisplay(caption, body, this);
	m_lastServerDisplay = bDisplay;

	if (m_displayQueue.getFirst() != m_lastServerDisplay)
		m_displayQueue.removeFirst();

	m_displayQueue.prepend(bDisplay);
	updateCenter();

	connect(bDisplay, SIGNAL(buttonCommand(QString)), this, SIGNAL(buttonCommand(QString)));
	connect(bDisplay, SIGNAL(buttonClose()), this, SLOT(displayDefault()));
}

void AtlantikBoard::displayButton(QString command, QString caption, bool enabled)
{
	if (BoardDisplay *display = dynamic_cast<BoardDisplay*>(m_lastServerDisplay))
		display->addButton(command, caption, enabled);
	else if (EstateDetails *display = dynamic_cast<EstateDetails*>(m_lastServerDisplay))
		display->addButton(command, caption, enabled);
}

void AtlantikBoard::addCloseButton()
{
	BoardDisplay *bDisplay = 0;
	EstateDetails *eDetails = 0;
	if ((bDisplay = dynamic_cast<BoardDisplay*>(m_lastServerDisplay)) && bDisplay != m_displayQueue.getLast())
		bDisplay->addCloseButton();
	else if ((eDetails = dynamic_cast<EstateDetails*>(m_lastServerDisplay)) && eDetails != m_displayQueue.getLast())
		eDetails->addCloseButton();
}

void AtlantikBoard::insertEstateDetails(Estate *estate)
{
	if (!estate)
		return;

	EstateDetails *eDetails = 0;

	// This might just be a update
	if ((eDetails = dynamic_cast<EstateDetails*>(m_lastServerDisplay)) && eDetails->estate() == estate)
	{
		eDetails->newUpdate();
		return;
	}

	if (m_displayQueue.getFirst() != m_lastServerDisplay)
		m_displayQueue.removeFirst();
	else
	{
		if (BoardDisplay *display = dynamic_cast<BoardDisplay*>(m_lastServerDisplay))
			display->addCloseButton();
		else if (EstateDetails *display = dynamic_cast<EstateDetails*>(m_lastServerDisplay))
			display->addCloseButton();
	}

	eDetails = new EstateDetails(estate, this);
	m_lastServerDisplay = eDetails;
	connect(eDetails, SIGNAL(buttonCommand(QString)), this, SIGNAL(buttonCommand(QString)));
	connect(eDetails, SIGNAL(buttonClose()), this, SLOT(displayDefault()));

	// Don't overwrite possible chance cards or previous estates
	int uid = 0;
	if (BoardDisplay *display = dynamic_cast<BoardDisplay*>(m_displayQueue.getFirst()))
		uid = 1;
	else if (EstateDetails *display = dynamic_cast<EstateDetails*>(m_displayQueue.getFirst()))
		uid = 1;
	
	m_displayQueue.insert(uid, eDetails);

	if (uid == 0)
		updateCenter();
}

void AtlantikBoard::prependEstateDetails(Estate *estate)
{
	if (!estate)
		return;

	EstateDetails *eDetails = new EstateDetails(estate, this);
	eDetails->addCloseButton();

	if (m_displayQueue.getFirst() != m_lastServerDisplay)
		m_displayQueue.removeFirst();

	m_displayQueue.prepend(eDetails);
	updateCenter();

	connect(eDetails, SIGNAL(buttonCommand(QString)), this, SIGNAL(buttonCommand(QString)));
	connect(eDetails, SIGNAL(buttonClose()), this, SLOT(displayDefault()));

}

void AtlantikBoard::updateCenter()
{
	QWidget *center = m_displayQueue.getFirst();
	m_gridLayout->addMultiCellWidget(center, 1, m_gridLayout->numRows()-2, 1, m_gridLayout->numCols()-2);
	center->show();
}
