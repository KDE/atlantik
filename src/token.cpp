#include <qpainter.h>

#include "token.h"

extern QColor kmonop_greenbg;

Token::Token(QWidget *parent, const char *name) : QWidget(parent, name)
{
	setBackgroundMode(NoBackground); // avoid flickering

	b_recreate = true;
	qpixmap = 0;

	setMinimumHeight(100);	
	setFixedSize(QSize(30, 30));
}

void Token::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(kmonop_greenbg);
		painter.setBrush(kmonop_greenbg);
		painter.drawRect(rect());

		painter.setPen(Qt::black);
		painter.setBrush(Qt::white);
		painter.drawEllipse(0, 0, width(), height());
		
		painter.drawText(width()/2, height()/2, "1");

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void Token::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}
