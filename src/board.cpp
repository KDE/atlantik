#include <qlayout.h>
#include <qpainter.h>

#warning remove iostream output
#include <iostream.h>

#include "board.moc"
#include "estateview.h"
#include "network.h"
#include "config.h"

extern QColor kmonop_dpurple, kmonop_lblue, kmonop_purple, kmonop_orange,
kmonop_red, kmonop_yellow, kmonop_green, kmonop_blue, kmonop_greenbg;
extern KMonopConfig kmonopConfig;

KMonopBoard::KMonopBoard(QWidget *parent, const char *name) : QWidget(parent, name)
{
	setMinimumWidth(320);
	setMinimumHeight(320);

	// Timer for token movement
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slotMoveToken()));
	m_resumeTimer = false;

	QGridLayout *layout = new QGridLayout(this, 25, 25);

	spacer = new QWidget(this);
	layout->addWidget(spacer, 24, 24); // SE

	center = new QWidget(this);
	layout->addMultiCellWidget(center, 3, 20, 3, 20);
	center->setBackgroundColor(kmonop_greenbg);

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
				color = kmonop_dpurple; break;
			case 6: case 8: case 9:
				color = kmonop_lblue; break;
			case 11: case 13: case 14:
				color = kmonop_purple; break;
			case 16: case 18: case 19:
				color = kmonop_orange; break;
			case 21: case 23: case 24:
				color = kmonop_red; break;
			case 26: case 27: case 29:
				color = kmonop_yellow; break;
			case 31: case 32: case 34:
				color = kmonop_green; break;
			case 37: case 39:
				color = kmonop_blue; break;

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

		if (i<10)
			orientation = North;
		else if (i<20)
			orientation = East;
		else if (i<30)
			orientation = South;
		else if (i<40)
			orientation = West;
			
		estate[i] = new EstateView(i, orientation, (color.isValid() || canBeOwned), color, icon, this, "estate");

		if (i==0)
			layout->addMultiCellWidget(estate[i], 21, 23, 21, 23);
		else if (i<10)
			layout->addMultiCellWidget(estate[i], 21, 23, 21-(i*2), 23-1-(i*2));
		else if (i==10)
			layout->addMultiCellWidget(estate[i], 21, 23, 0, 2);
		else if (i<20)
			layout->addMultiCellWidget(estate[i], 21-((i-10)*2), 23-1-((i-10)*2), 0, 2);
		else if (i==20)
			layout->addMultiCellWidget(estate[i], 0, 2, ((i-20)*2), 2+((i-20)*2));
		else if (i<30)
			layout->addMultiCellWidget(estate[i], 0, 2, 1+((i-20)*2), 2+((i-20)*2));
		else if (i==30)
			layout->addMultiCellWidget(estate[i], 0, 2, 21, 23);
		else
			layout->addMultiCellWidget(estate[i], ((i-30)*2)+1, ((i-30)*2)+2, 21, 23);
	}

	connect(gameNetwork, SIGNAL(msgPlayerUpdateLocation(int, int, bool)), this, SLOT(slotMsgPlayerUpdateLocation(int, int, bool)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateName(int, QString)), this, SLOT(slotMsgEstateUpdateName(int, QString)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateHouses(int, int)), this, SLOT(slotMsgEstateUpdateHouses(int, int)));
	connect(gameNetwork, SIGNAL(msgEstateUpdateMortgage(int, bool)), this, SLOT(slotMsgEstateUpdateMortgage(int, bool)));

	QString label;
	for(i=0;i<MAXPLAYERS;i++)
	{
		label.setNum(i);
		token[i] = new Token(label, this, "token");
		jumpToken(token[i], 0);
		token[i]->hide();
	}
}

void KMonopBoard::jumpToken(Token *token, int to)
{
	int x = estate[to]->geometry().center().x() - (token->width()/2);
	int y = estate[to]->geometry().center().y() - (token->height()/2);

	token->setLocation(to);
	token->setGeometry(x, y, token->width(), token->height());

	// Confirm location to server.
	gameNetwork->cmdTokenConfirmation(to);
}

void KMonopBoard::moveToken(Token *token, int dest)
{
	cout << "moving piece from " << token->location() << " to " << dest << endl;

	// Set token destination
	move_token = token;
	move_token->setDestination(dest);

	// Start timer
	m_timer->start(10);
}

void KMonopBoard::setOwned(int id, bool owned)
{
	if (id>=0 && id<40)
		estate[id]->setOwned(owned);
}

void KMonopBoard::raiseToken(int id)
{
	if (id>=0 && id<MAXPLAYERS && token[id]!=0)
		token[id]->raise();
}

void KMonopBoard::indicateUnownedChanged()
{
	int i=0;

	for (i=0;i<40;i++)
		if (estate[i]!=0)
			estate[i]->updatePE();
}

void KMonopBoard::redrawEstates()
{
	int i=0;

	for (i=0;i<40;i++)
		if (estate[i]!=0 && ( !(estate[i]->owned()) || estate[i]->mortgaged() ))
			estate[i]->redraw();
}

void KMonopBoard::slotMoveToken()
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
	cout << "we are at " << curX << "," << curY << endl;

	// Where do we want to go today?
	dest = move_token->location() + 1;
	if (dest==40)
		dest = 0;
	cout << "going from " << move_token->location() << " to " << dest << endl;

	destX = estate[dest]->geometry().center().x() - (move_token->width()/2);
	destY = estate[dest]->geometry().center().y() - (move_token->height()/2);
	cout << "going to " << destX << "," << destY << endl;

	if (curX == destX && curY == destY)
	{
		// We have arrived at our destination!
		move_token->setLocation(dest);

		// We need to confirm passing Go and arriving at our final
		// destination to the server.
		if (move_token->destination() == move_token->location())
		{
			gameNetwork->cmdTokenConfirmation(move_token->location());

			// We have arrived at our _final_ destination!
			m_timer->stop();
			move_token = 0;
		}
		else if (move_token->location() == 0)
			gameNetwork->cmdTokenConfirmation(move_token->location());

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

void KMonopBoard::resizeEvent(QResizeEvent *e)
{
	// Stop moving tokens, slotResizeAftermath will re-enable this
	if (m_timer!=0 && m_timer->isActive())
	{
		m_timer->stop();
		m_resumeTimer=true;
	}

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

	// Timer to reinit the gameboard _after_ resizeEvent
	QTimer::singleShot(0, this, SLOT(slotResizeAftermath()));
}

void KMonopBoard::slotResizeAftermath()
{
	// Move tokens back to their last known location (this has to be done
	// _after_ resizeEvent has returned to make sure we have the correct
	// adjusted estate geometries.

	for(int i=0;i<MAXPLAYERS;i++)
	{
		if (token[i]!=0)
			jumpToken(token[i], token[i]->location());
	}

	// Restart the timer that was stopped in resizeEvent
	if (m_resumeTimer && m_timer!=0 && !m_timer->isActive())
	{
		m_timer->start(10);
		m_resumeTimer=false;
	}
}

void KMonopBoard::slotMsgPlayerUpdateLocation(int playerid, int location, bool direct)
{
	if (playerid>=0 && playerid < MAXPLAYERS && token[playerid]!=0)
	{
		if (token[playerid]->isHidden())
			token[playerid]->show();

		// Only take action when location has changed
		if (token[playerid]->location() != location)
		{
			if (direct || kmonopConfig.animateToken==false)
				jumpToken(token[playerid], location);
			else
				moveToken(token[playerid], location);
		}
	}
}

void KMonopBoard::slotMsgEstateUpdateName(int estateid, QString name)
{
	if (estateid>=0 && estateid < 40 && estate[estateid]!=0)
		estate[estateid]->setName(name);
}

void KMonopBoard::slotMsgEstateUpdateHouses(int estateid, int houses)
{
	if (estateid>=0 && estateid < 40 && estate[estateid]!=0)
		estate[estateid]->setHouses(houses);
}

void KMonopBoard::slotMsgEstateUpdateMortgage(int estateid, bool mortgaged)
{
	if (estateid>=0 && estateid < 40 && estate[estateid]!=0)
		estate[estateid]->setMortgaged(mortgaged);
}
