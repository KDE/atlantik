#include <qpainter.h>

#include <kdebug.h>

#include <player.h>
#include <estate.h>
#include <auction.h>

#include <atlantik_network.h>

#include "board.moc"
#include "estateview.h"
#include "config.h"
#include "display_widget.h"
#include "auction_widget.h"

extern AtlantikConfig atlantikConfig;

AtlantikBoard::AtlantikBoard(AtlanticCore *atlanticCore, int maxEstates, QWidget *parent, const char *name) : QWidget(parent, name)
{
//	setMinimumWidth(320);
//	setMinimumHeight(320);

	m_atlanticCore = atlanticCore;

	m_maxEstates = maxEstates;

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

	m_center = 0;
	displayCenter();

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

/*
	QString label;
	for(i=0;i<MAXPLAYERS;i++)
	{
		label.setNum(i);
	}
*/
	kdDebug() << "ending board ctor" << endl;
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

void AtlantikBoard::addEstateView(Estate *estate)
{
	QString icon = QString();
	int estateId = estate->estateId(), orientation = North;
	int sideLen = m_gridLayout->numRows() - 1;

	if (estateId < sideLen)
		orientation = North;
	else if (estateId < 2*sideLen)
		orientation = East;
	else if (estateId < 3*sideLen)
		orientation = South;
	else //if (estateId < 4*sideLen)
		orientation = West;
	
	EstateView *estateView = new EstateView(estate, orientation, icon, this, "estateview");
	m_estateViews.append(estateView);

	connect(estate, SIGNAL(changed()), estateView, SLOT(estateChanged()));
	connect(estateView, SIGNAL(estateToggleMortgage(Estate *)), estate, SIGNAL(estateToggleMortgage(Estate *)));
	connect(estateView, SIGNAL(LMBClicked(Estate *)), estate, SIGNAL(LMBClicked(Estate *)));
	connect(estateView, SIGNAL(estateHouseBuy(Estate *)), estate, SIGNAL(estateHouseBuy(Estate *)));
	connect(estateView, SIGNAL(estateHouseSell(Estate *)), estate, SIGNAL(estateHouseSell(Estate *)));
	connect(estateView, SIGNAL(newTrade(Player *)), estate, SIGNAL(newTrade(Player *)));

	if (estateId<sideLen)
		m_gridLayout->addWidget(estateView, sideLen, sideLen-estateId);
	else if (estateId<2*sideLen)
		m_gridLayout->addWidget(estateView, 2*sideLen-estateId, 0);
	else if (estateId<3*sideLen)
		m_gridLayout->addWidget(estateView, 0, estateId-2*sideLen);
	else
		m_gridLayout->addWidget(estateView, estateId-3*sideLen, sideLen);

	estateView->show();
}

void AtlantikBoard::addAuctionWidget(Auction *auction)
{
	if (m_center != 0)
		delete m_center;

	m_center = new AuctionWidget(m_atlanticCore, auction, this);
	m_gridLayout->addMultiCellWidget(m_center, 1, m_gridLayout->numRows()-2, 1, m_gridLayout->numCols()-2);
	m_center->show();

	connect(auction, SIGNAL(completed()), this, SLOT(displayCenter()));
}

void AtlantikBoard::addToken(Player *player)
{
	kdDebug() << "AtlantikBoard::addToken" << endl;

	Token *token = new Token(player, this, "token");
	tokenMap[player->playerId()] = token;

	connect(player, SIGNAL(changed()), token, SLOT(playerChanged()));

	// Hide and don't position, because as long as we haven't reentered the
	// event loop, the estate geometries are not correct anyway. Is this
	// even solvable having playerupdate and estateupdate in the same
	// initial message from monopd?
	token->hide();
//	token->setLocation(0);
//	token->setDestination(0);

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
	Token *token = tokenMap[playerId];
	
	if (estateView && token)
	{
		if (token->isHidden())
			token->show();

		// Only take action when location has changed
		if (token->location() != estateId)
		{
			if (directMove)
				jumpToken(token, estateView, false);
			else if (atlantikConfig.animateToken==false)
				jumpToken(token, estateView);
			else
				moveToken(token, estateId);
		}
	}
*/
}

void AtlantikBoard::jumpToken(Token *token, EstateView *estateView, bool confirm)
{
#warning AtlantikBoard::jumpToken doesnt know about player->inJail()
	kdDebug() << "AtlantikBoard::jumpToken(" << token->destination() << ", "  << confirm << ")" << endl;

	if (estateView)
	{
		int x = estateView->geometry().center().x() - (token->width()/2);
		int y = estateView->geometry().center().y() - (token->height()/2);
		kdDebug() << "jumpToken says x is " << x << " and y is " << y << endl;

		token->setLocation(estateView);
		token->setGeometry(x, y, token->width(), token->height());

		// Confirm location to server.
#warning port
//		if (confirm)
//			emit tokenConfirmation(estateId);
	}
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

void AtlantikBoard::indicateUnownedChanged()
{
	EstateView *estateView;
	for (QPtrListIterator<EstateView> i(m_estateViews); *i; ++i)
	{
		if ((estateView = dynamic_cast<EstateView*>(*i)))
			estateView->updatePE();
	}
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

	for (unsigned int tokenId=0 ; tokenId < tokenMap.size() ; tokenId++)
	{
#warning port
//		if (Token *token = tokenMap[tokenId])
//			jumpToken(token, token->location(), false);
	}

	// Restart the timer that was stopped in resizeEvent
	if (m_resumeTimer && m_timer!=0 && !m_timer->isActive())
	{
		m_timer->start(15);
		m_resumeTimer=false;
	}
}

void AtlantikBoard::displayCenter()
{
	if (m_center != 0)
		delete m_center;

	m_center = new QWidget(this);
	m_gridLayout->addMultiCellWidget(m_center, 1, m_gridLayout->numRows()-2, 1, m_gridLayout->numCols()-2);
	m_center->show();
}

void AtlantikBoard::slotDisplayCard(QString type, QString description)
{
	kdDebug() << "displayCard(" << type << ", " << description << ")" << endl;

	if (m_center != 0)
		delete m_center;
	
	m_center = new BoardDisplay(type, description, this);
	m_gridLayout->addMultiCellWidget(m_center, 1, m_gridLayout->numRows()-2, 1, m_gridLayout->numCols()-2);
	m_center->show();

	QTimer::singleShot(3000, this, SLOT(displayCenter()));
}
