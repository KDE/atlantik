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
