#include <qpainter.h>

#include "portfolioview.h"
#include "portfolioestate.h"

extern QColor kmonop_dpurple, kmonop_lblue, kmonop_purple, kmonop_orange,
kmonop_red, kmonop_yellow, kmonop_green, kmonop_blue, kmonop_greenbg,
kmonop_lgray;

PortfolioView::PortfolioView(QWidget *parent, const char *name) : QWidget(parent, name)
{
	b_recreate = true;
	qpixmap = 0;

	setFixedSize(QSize(150, 75));
	setBackgroundColor(Qt::white);
	
	lname = new QLabel(this);
	lname->setAlignment(Qt::AlignLeft);
	lname->setGeometry(5,0,(width()-5),height());
	lname->setBackgroundColor(kmonop_lgray);
	lname->setMinimumSize(lname->sizeHint());
	lname->setMaximumWidth(width()-10);
	lname->setMaximumHeight(15);
	lname->show();
/*
	lcash = new QLabel(this);
	lcash->setAlignment(Qt::AlignRight);
	lcash->setGeometry(5+(width()-5)/2,0,(width()-5),height());
	lcash->setBackgroundColor(Qt::white);
	lcash->setMinimumSize(lcash->sizeHint());
	lcash->setMaximumWidth(width()/2);
	lcash->setMaximumHeight(15);
	lcash->show();
	lcash->setText("$ 1,500");
*/	
	PortfolioEstate *estate;
	QColor current;

	int i=0,j=0,x=0,y=0,w=8;
//	paint.drawRect(rect());
	for(i=0;i<8;i++)
	{
		switch(i)
		{
	 		case 0:
			 	current=kmonop_dpurple; break;
			case 1:
		 		current=kmonop_lblue; break;
			case 2:
				current=kmonop_purple; break;
			case 3:
				current=kmonop_orange; break;
			case 4:
				current=kmonop_red; break;
			case 5:
				current=kmonop_yellow; break;
			case 6:
				current=kmonop_green; break;
			case 7:
				current=kmonop_blue; break;
		}
		estate = new PortfolioEstate(this);
		x = 5+(18*(i%w));
		y = 2+lname->height()+(i>=w ? 22: 0);
		estate->setGeometry(x, y, estate->width(), estate->height());
		estate->setColor(current);
		if (j++%3==0 || j==14 || j==16)
			estate->setOwned(true);
		estate->show();

		estate = new PortfolioEstate(this);
		x = 7+(18*(i%w));
		y = 6+lname->height()+(i>=w ? 22: 0);
		estate->setGeometry(x, y, estate->width(), estate->height());
		estate->setColor(current);
		if (j++%2==0 || j==14 || j==16)
			estate->setOwned(true);
		estate->show();

		if (i>0 && i<7)
		{
			estate = new PortfolioEstate(this);
			x = 9+(18*(i%w));
			y = 10+lname->height()+(i>=w ? 22: 0);
			estate->setGeometry(x, y, estate->width(), estate->height());
				estate->setColor(current);
			if (j++%2==0 || j==14 || j==16)
				estate->setOwned(true);
			estate->show();
		}
	}
	for (i=0;i<8;i++)
	{
		estate = new PortfolioEstate(this);
		x = 5+(14*i);
		y = 26+lname->height();
		estate->setGeometry(x, y, estate->width(), estate->height());
		estate->setColor(Qt::black);
		estate->show();
	}
}

void PortfolioView::setName(const char *n)
{
	name.setLatin1(n, strlen(n));
	lname->setText(n);
}

void PortfolioView::paintEvent(QPaintEvent *)
{
	return;
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}
