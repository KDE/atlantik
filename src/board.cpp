#include <qpainter.h>

#include <kdebug.h>

#include "board.moc"
#include "estateview.h"
#include "estate.h"
#include "player.h"
#include "network.h"
#include "config.h"

extern QColor atlantik_greenbg;
extern AtlantikConfig atlantikConfig;

AtlantikBoard::AtlantikBoard(QWidget *parent, const char *name) : QWidget(parent, name)
{
//	setMinimumWidth(320);
//	setMinimumHeight(320);

	// Timer for token movement
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slotMoveToken()));
	m_resumeTimer = false;

	m_gridLayout = new QGridLayout(this, 11, 11);
	for(int i=0;i<=10;i++)
	{
		if (i==0 || i==10)
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
//	m_gridLayout->addWidget(spacer, 11, 11); // SE

	center = new QWidget(this);
	m_gridLayout->addMultiCellWidget(center, 1, 9, 1, 9);
	center->setBackgroundColor(atlantik_greenbg);

	int i=0, orientation=North;

	QColor color;
	QString icon;
	bool canBeOwned;

	for (i=0;i<40;i++)
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
		token[i] = new Token(label, this, "token");
		jumpToken(token[i], 0, false);
		token[i]->hide();
	}
*/
	kdDebug() << "ending board ctor" << endl;
}

void AtlantikBoard::addEstateView(Estate *estate)
{
	bool canBeOwned = false;
	QString icon = QString();
	int estateId = estate->estateId(), orientation = North;

	if (estateId < 10)
		orientation = North;
	else if (estateId < 20)
		orientation = East;
	else if (estateId < 30)
		orientation = South;
	else if (estateId < 40)
		orientation = West;
	
	EstateView *estateView = new EstateView(estate, orientation, icon, this, "estateview");
	estateViewMap[estateId] = estateView;

	connect(estate, SIGNAL(changed()), estateView, SLOT(estateChanged()));
	connect(estateView, SIGNAL(estateToggleMortgage(int)), estate, SIGNAL(estateToggleMortgage(int)));
	connect(estateView, SIGNAL(estateHouseBuy(int)), estate, SIGNAL(estateHouseBuy(int)));
	connect(estateView, SIGNAL(estateHouseSell(int)), estate, SIGNAL(estateHouseSell(int)));

	if (estateId<10)
		m_gridLayout->addWidget(estateView, 10, 10-estateId);
	else if (estateId<20)
		m_gridLayout->addWidget(estateView, 20-estateId, 0);
	else if (estateId<30)
		m_gridLayout->addWidget(estateView, 0, estateId-20);
	else
		m_gridLayout->addWidget(estateView, estateId-30, 10);

	estateView->show();
}

void AtlantikBoard::addToken(Player *player)
{
	kdDebug() << "AtlantikBoard::addToken" << endl;

	Token *token = new Token(player, this, "token");
	tokenMap[player->playerId()] = token;

//	connect(player, SIGNAL(changed()), token, SLOT(playerChanged()));

	// Hide and don't position, because as long as we haven't reentered the
	// event loop, the estate geometries are not correct anyway. Is this
	// even solvable having playerupdate and estateupdate in the same
	// initial message from monopd?
	token->hide();
	jumpToken(token, 0, false);
}

void AtlantikBoard::jumpToken(Token *token, int estateId, bool confirm)
{
	kdDebug() << "AtlantikBoard::jumpToken(" << estateId << ", "  << confirm << ")" << endl;

	if (EstateView *estateView = estateViewMap[estateId])
	{
		int x = estateView->geometry().center().x() - (token->width()/2);
		int y = estateView->geometry().center().y() - (token->height()/2);
		kdDebug() << "jumpToken says x is " << x << " and y is " << y << endl;

		token->setLocation(estateId);
		token->setGeometry(x, y, token->width(), token->height());

		// Confirm location to server.
		if (confirm)
			emit tokenConfirmation(estateId);
	}
}

void AtlantikBoard::moveToken(Token *token, int estateId)
{
	if ( token==0 )
		return;
	
	kdDebug() << "moving piece from " << token->location() << " to " << estateId << endl;

	// Set token destination
	move_token = token;
	move_token->setDestination(estateId);

	// Start timer
	m_timer->start(10);
}

void AtlantikBoard::raiseToken(int id)
{
#warning port AtlantikBoard::raiseToken
//	if (id>=0 && id<MAXPLAYERS && token[id]!=0)
//		token[id]->raise();
}

void AtlantikBoard::indicateUnownedChanged()
{
#warning port Board::indicateUnownedChaged
	int i=0;

//	for (i=0;i<40;i++)
//		if (estate[i]!=0)
//			estate[i]->updatePE();
}

void AtlantikBoard::slotMoveToken()
{
	int dest;
	int destX,destY,curX,curY;

	// Do we actually have a token to move?
	if (move_token==0)
	{
		m_timer->stop();
		return;
	}

	// Where are we?
	curX = move_token->geometry().x();
	curY = move_token->geometry().y();
	kdDebug() << "we are at " << curX << "," << curY << endl;

	// Where do we want to go today?
	dest = move_token->location() + 1;
	if (dest==40)
		dest = 0;
	kdDebug() << "going from " << move_token->location() << " to " << dest << endl;

#warning port Board::slotMoveToken
//	destX = estate[dest]->geometry().center().x() - (move_token->width()/2);
//	destY = estate[dest]->geometry().center().y() - (move_token->height()/2);
	kdDebug() << "going to " << destX << "," << destY << endl;

	if (curX == destX && curY == destY)
	{
		// We have arrived at our destination!
		move_token->setLocation(dest);

		// We need to confirm passing Go and arriving at our final
		// destination to the server.
		if (move_token->destination() == move_token->location())
		{
			emit tokenConfirmation(move_token->location());

			// We have arrived at our _final_ destination!
			m_timer->stop();
			move_token = 0;
		}
		else if (move_token->location() == 0)
			emit tokenConfirmation(move_token->location());

		return;
	}

	if (curX!=destX)
	{
		if (destX > curX)
			move_token->setGeometry(curX+1, curY, move_token->width(), move_token->height());
		else
			move_token->setGeometry(curX-1, curY, move_token->width(), move_token->height());
	}
	if (curY!=destY)
	{
		if (destY > curY)
			move_token->setGeometry(curX, curY+1, move_token->width(), move_token->height());
		else
			move_token->setGeometry(curX, curY-1, move_token->width(), move_token->height());
	}
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
#warning port AtlantikBoard::slotResizeAftermath
	// Move tokens back to their last known location (this has to be done
	// _after_ resizeEvent has returned to make sure we have the correct
	// adjusted estate geometries.

/*
	for(int i=0;i<MAXPLAYERS;i++)
	{
		if (token[i]!=0)
			jumpToken(token[i], token[i]->location(), false);
	}
*/

	// Restart the timer that was stopped in resizeEvent
	if (m_resumeTimer && m_timer!=0 && !m_timer->isActive())
	{
		m_timer->start(10);
		m_resumeTimer=false;
	}
}

void AtlantikBoard::slotMsgPlayerUpdateLocation(int playerId, int estateId, bool directMove)
{
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
				jumpToken(token, estateId, false);
			else if (atlantikConfig.animateToken==false)
				jumpToken(token, estateId);
			else
				jumpToken(token, estateId);
#warning reenable moveToken
//				moveToken(token, estateId);
		}
	}
}

