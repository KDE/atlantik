#include <qpainter.h>

#include <kdebug.h>

#include "board.moc"
#include "estateview.h"
#include "estate.h"
#include "network.h"
#include "config.h"

extern QColor atlantik_dpurple, atlantik_lblue, atlantik_purple, atlantik_orange,
atlantik_red, atlantik_yellow, atlantik_green, atlantik_blue, atlantik_greenbg;
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
			case 1: case 3:
				color = atlantik_dpurple; break;
			case 6: case 8: case 9:
				color = atlantik_lblue; break;
			case 11: case 13: case 14:
				color = atlantik_purple; break;
			case 16: case 18: case 19:
				color = atlantik_orange; break;
			case 21: case 23: case 24:
				color = atlantik_red; break;
			case 26: case 27: case 29:
				color = atlantik_yellow; break;
			case 31: case 32: case 34:
				color = atlantik_green; break;
			case 37: case 39:
				color = atlantik_blue; break;

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

	QString label;
	for(i=0;i<MAXPLAYERS;i++)
	{
		label.setNum(i);
		token[i] = new Token(label, this, "token");
		jumpToken(token[i], 0, false);
		token[i]->hide();
	}
	kdDebug() << "ending board ctor" << endl;
}

void AtlantikBoard::addEstateView(Estate *estate)
{
	bool canBeOwned = false;
	QString icon = QString();

	EstateView *estateView = new EstateView(estate, North, icon, this, "estateview");
	connect(estate, SIGNAL(changed()), estateView, SLOT(estateChanged()));

	int estateId = estate->estateId();
	if (estateId<10)
	{
		m_gridLayout->addWidget(estateView, 10, 10-estateId);
//		orientation = North;
	}
	else if (estateId<20)
	{
		m_gridLayout->addWidget(estateView, 20-estateId, 0);
//		orientation = East;
	}
	else if (estateId<30)
	{
		m_gridLayout->addWidget(estateView, 0, estateId-20);
//		orientation = South;
	}
	else
	{
		m_gridLayout->addWidget(estateView, estateId-30, 10);
//		orientation = West;
	}

	estateView->show();
}

void AtlantikBoard::jumpToken(Token *token, int destination, bool confirm)
{
#warning port Board::jumpToken
	int x=0, y=0;
//	int x = estate[destination]->geometry().center().x() - (token->width()/2);
//	int y = estate[destination]->geometry().center().y() - (token->height()/2);

	token->setLocation(destination);
	token->setGeometry(x, y, token->width(), token->height());

	// Confirm location to server.
	if (confirm)
		emit tokenConfirmation(destination);
}

void AtlantikBoard::moveToken(Token *token, int destination)
{
	kdDebug() << "moving piece from " << token->location() << " to " << destination << endl;

	// Set token destination
	move_token = token;
	move_token->setDestination(destination);

	// Start timer
	m_timer->start(10);
}

void AtlantikBoard::setOwned(int estateId, bool byAny, bool byThisClient)
{
#warning port Board::setOwned
//	if (estateId>=0 && estateId<40)
//		estate[estateId]->setOwned(byAny, byThisClient);
}

void AtlantikBoard::raiseToken(int id)
{
	if (id>=0 && id<MAXPLAYERS && token[id]!=0)
		token[id]->raise();
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
	// Move tokens back to their last known location (this has to be done
	// _after_ resizeEvent has returned to make sure we have the correct
	// adjusted estate geometries.

	for(int i=0;i<MAXPLAYERS;i++)
	{
		if (token[i]!=0)
			jumpToken(token[i], token[i]->location(), false);
	}

	// Restart the timer that was stopped in resizeEvent
	if (m_resumeTimer && m_timer!=0 && !m_timer->isActive())
	{
		m_timer->start(10);
		m_resumeTimer=false;
	}
}

void AtlantikBoard::slotMsgPlayerUpdateLocation(int playerid, int location, bool direct)
{
	if (playerid>=0 && playerid < MAXPLAYERS && token[playerid]!=0)
	{
		if (token[playerid]->isHidden())
			token[playerid]->show();

		// Only take action when location has changed
		if (token[playerid]->location() != location)
		{
			if (direct)
				jumpToken(token[playerid], location, false);
			else if(atlantikConfig.animateToken==false)
				jumpToken(token[playerid], location);
			else
				moveToken(token[playerid], location);
		}
	}
}

