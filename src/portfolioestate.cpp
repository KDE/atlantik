#include <qpainter.h>
#include <qcolor.h>

#include "portfolioestate.h"

extern QColor kmonop_lgray, kmonop_dgray;

PortfolioEstate::PortfolioEstate(QWidget *parent, const char *name) : QWidget(parent, name)
{
    QColor kmonop_lgray(187, 187, 187);
    color = kmonop_lgray;
    QSize s(12,14);
    setFixedSize(s);
	owned = false;
    b_recreate = true;
    qpixmap=0;
}

void PortfolioEstate::setColor(const QColor &c)
{
	color = c;
}

void PortfolioEstate::setOwned(bool o)
{
	owned = o;
}

void PortfolioEstate::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		if (qpixmap!=0)
			delete qpixmap;
		qpixmap = new QPixmap(width(), height());
		
		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(kmonop_lgray);
		painter.setBrush(white);
		painter.drawRect(rect());
		if (owned)
		{
			painter.setPen(kmonop_dgray);
			for (int x=4;x<=10;x+=2)
				painter.drawLine(2, x, 9, x);

			painter.setPen(Qt::white);
			painter.drawPoint(6, 4);
			painter.drawPoint(7, 6);
			painter.drawPoint(5, 8);
			painter.drawPoint(8, 10);

			painter.setPen(color);	
			painter.setBrush(color);
		}
		else
		{
			painter.setPen(kmonop_lgray);	
			painter.setBrush(kmonop_lgray);
		}
		painter.drawRect(0,0,width(),3);
		
		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}
