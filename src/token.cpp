#include <qpainter.h>

#include "token.h"

extern QColor kmonop_greenbg;

Token::Token(QString id, QWidget *parent, const char *name) : QWidget(parent, name)
{
	setBackgroundMode(NoBackground); // avoid flickering

	b_recreate = true;
	qpixmap = 0;

	setMinimumHeight(100);	
	setFixedSize(QSize(15, 15));
	
	myId = id;
	myLoc = myDest = 0;
}

void Token::setLocation(int _loc)
{
	myLoc = _loc;
}

int Token::location()
{
	return myLoc;
}

void Token::setDestination(int _dest)
{
	myDest = _dest;
}

int Token::destination()
{
	return myDest;
}

void Token::moveTo(int loc)
{
	int X = geometry().x();
	int Y = geometry().y();
/*
	int x = estate[loc]->geometry().center().x() - (width()/2);
	int y = estate[loc]->geometry().center().y() - (height()/2);
	while (X!=x && Y!=y)
	{
		if (X<x)
			token[id]->setGeometry(x-1, y, token[id]->width(), token[id]->height());
		else if (X>x)
			token[id]->setGeometry(x+1, y, token[id]->width(), token[id]->height());
		if (Y<y)
			token[id]->setGeometry(x, y-1, token[id]->width(), token[id]->height());
		else if (Y>y)
			token[id]->setGeometry(x, y+1, token[id]->width(), token[id]->height());

		sleep(1);
	}
	token[id]->setGeometry(x, y, width(), height());
*/
}

void Token::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(Qt::black);
		painter.setBrush(Qt::white);
		painter.drawRect(rect());

		painter.drawText(2, height()-2, myId);

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}
