#include <qpainter.h>
#include <qcolor.h>

#include "portfolioestate.h"

extern QColor kmonop_lgray, kmonop_dgray;

PortfolioEstate::PortfolioEstate(QWidget *parent, const char *name) : QWidget(parent, name)
{
    color = kmonop_lgray;
    QSize s(PE_WIDTH, PE_HEIGHT);
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
	if (o!=owned)
	{
		owned = o;
		b_recreate = true;
		update();
	}
}

void PortfolioEstate::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
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
			for (int y=5;y<=13;y+=2)
				painter.drawLine(2, y, 10, y);

			painter.setPen(Qt::white);
			painter.drawPoint(8, 5);
			painter.drawPoint(8, 7);
			painter.drawPoint(8, 9);
			painter.drawPoint(5, 11);
			painter.drawPoint(9, 11);
			painter.drawPoint(3, 13);
			painter.drawPoint(10, 13);

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
