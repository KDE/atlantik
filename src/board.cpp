#include <qlayout.h>
#include <qpainter.h>

#warning remove iostream output
#include <iostream.h>

#include "board.moc"
#include "estateview.h"

extern QColor kmonop_dpurple, kmonop_lblue, kmonop_purple, kmonop_orange,
kmonop_red, kmonop_yellow, kmonop_green, kmonop_blue, kmonop_greenbg;

KMonopBoard::KMonopBoard(GameNetwork *_nw, QWidget *parent, const char *name) : QWidget(parent, name)
{
	netw = _nw;

	setMinimumWidth(160);
	setMinimumHeight(160);

	// Timer for token movement
	qtimer = new QTimer(this);
	connect(qtimer, SIGNAL(timeout()), this, SLOT(slotMoveToken()));
	resume_timer = false;

	QGridLayout *layout = new QGridLayout(this, 25, 25);

	spacer = new QWidget(this);
	layout->addWidget(spacer, 24, 24); // SE

	int i=0, orientation=North;

	QColor color;
	QString icon;

	for (i=0;i<40;i++)
	{
		color = QColor();
		icon = QString();

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
				break;
			case 7: case 36:
				icon = QString("qmark-red.png");
				break;
			case 22:
				icon = QString("qmark-blue.png");
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
			
		estate[i] = new EstateView(orientation, color, icon, this, "estate");
		if (color.isValid())
			estate[i]->setOwned(false);

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

	connect(netw, SIGNAL(msgMoveToken(int, int)), this, SLOT(slotMsgMoveToken(int, int)));
	connect(netw, SIGNAL(msgPlayerUpdate(QDomNode)), this, SLOT(slotMsgPlayerUpdate(QDomNode)));

	for(i=0;i<MAXPLAYERS;i++)
		token[i]=0;
}

void KMonopBoard::jumpToken(Token *token, int to)
{
	int x = estate[to]->geometry().center().x() - (token->width()/2);
	int y = estate[to]->geometry().center().y() - (token->height()/2);

	token->setLocation(to);
	token->setGeometry(x, y, token->width(), token->height());
}

void KMonopBoard::moveToken(Token *token, int dest)
{
	cout << "moving piece from " << token->location() << " to " << dest << endl;

	// Set token destination
	move_token = token;
	move_token->setDestination(dest);

	// Start timer
	qtimer->start(10);
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

void KMonopBoard::slotMoveToken()
{
	int dest;
	int destX,destY,curX,curY;

	// Do we actually have a token to move?
	if (move_token==0)
	{
		qtimer->stop();
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
		QString msg(".t"), loc;
		loc.setNum(dest);
		msg.append(loc);
		cout << "msg is " << msg << endl;
		netw->writeData(msg.latin1());

		if (move_token->destination() == move_token->location())
		{
			// We have arrived at our _final_ destination!
			qtimer->stop();
			move_token = 0;
		}
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
	if (qtimer!=0 && qtimer->isActive())
	{
		qtimer->stop();
		resume_timer=true;
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
		{
			jumpToken(token[i], token[i]->location());
		}
	}

	// Restart the timer that was stopped in resizeEvent
	if (resume_timer && qtimer!=0 && !qtimer->isActive())
	{
		qtimer->start(10);
		resume_timer=false;
	}
}

void KMonopBoard::slotMsgMoveToken(int player, int location)
{
	// Adjust player because our array is 0-indexed.
	player--;

	// Maximum of six players, right? Check with monopd, possibly fetch
	// number first and make portfolio overviews part of QHLayout to allow
	// any theoretically number.

	if (player >= 0 && player < MAXPLAYERS)
	{
		if(token[player]==0)
		{
			token[player] = new Token(QString("" + player), this, "token");
			jumpToken(token[player], 0);
			token[player]->show();
		}

		// Only move when location has changed
		if (token[player]->location() != location)
			moveToken(token[player], location);
	}
}

void KMonopBoard::slotMsgPlayerUpdate(QDomNode n)
{
	QDomAttr a_id, a_location;
	QDomElement e;
	int id=0;
	
	e = n.toElement();
	if(!e.isNull())
	{
		a_id = e.attributeNode(QString("id"));
		if (a_id.isNull())
			return;

		// Maximum of six players, right? Check with monopd, possibly fetch
		// number first and make portfolio overviews part of QHLayout to
		// allow any theoretically number.
		id = a_id.value().toInt() - 1;
		if (id < MAXPLAYERS)
		{
			if(token[id]==0)
			{
				token[id] = new Token(a_id.value(), this, "token");
				jumpToken(token[id], 0);
				token[id]->show();
			}
		}
	}
}
