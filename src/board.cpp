#include <qlayout.h>
#include <qpainter.h>

#warning remove
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

	// Timer to reinit the gameboard _after_ resizeEvent
	qtimer_resize =  new QTimer(this);
	connect(qtimer_resize, SIGNAL(timeout()), this, SLOT(slotResizeAftermath()));

	QGridLayout *layout = new QGridLayout(this, 25, 25);

	spacer = new QWidget(this);
	layout->addWidget(spacer, 24, 24); // SE

	int i=0, orientation=North;

	QColor color;
	for (i=0;i<40;i++)
	{
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
			default:
				color = QColor();
		}

		if (i<10)
			orientation = North;
		else if (i<20)
			orientation = East;
		else if (i<30)
			orientation = South;
		else if (i<40)
			orientation = West;
			
		estate[i] = new EstateView(orientation, color, this, "estate");

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

//		estate[i]->setMinimumWidth(w);
//		estate[i]->setMinimumHeight(h);
	}

	connect(netw, SIGNAL(msgPlayerUpdate(QDomNode)), this, SLOT(slotMsgPlayerUpdate(QDomNode)));

	for(i=0;i<MAXPLAYERS;i++)
		token[i]=0;
}

void KMonopBoard::jumpToken(Token *token, int to)
{
	int x=0, y=0;
	
	x = estate[to]->geometry().center().x() - (token->width()/2);
	y = estate[to]->geometry().center().y() - (token->height()/2);

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

void KMonopBoard::slotMoveToken()
{
	int dest;
	int destX,destY,curX,curY;

	// Do we actually have a token to move?
	if (move_token==0)
		return;

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

		if (move_token->destination() == move_token->location())
		{
			// We have arrived at our _final_ destination!
			qtimer->stop();
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

	// Make sure we enter slotResizeAftermath
	if (qtimer_resize!=0 && !qtimer->isActive())
		qtimer_resize->start(0);
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

	// Stop resize timer
	if (qtimer_resize!=0 && qtimer->isActive())
		qtimer_resize->stop();

	// Restart the timer that was stopped in resizeEvent
	if (resume_timer && qtimer!=0 && !qtimer->isActive())
	{
		qtimer->start(10);
		resume_timer=false;
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

			a_location = e.attributeNode(QString("location"));
			// Only move when location has changed
			if (!a_location.isNull() && token[id]->location()!=a_location.value().toInt())
				moveToken(token[id], a_location.value().toInt());
		}
	}
}
