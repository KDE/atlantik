#include <qpainter.h>

#include "portfolioview.h"

#define PE_DISTW	4
#define	PE_DISTH	4
#define PE_SPACE	2
#define PE_MARGINW	5
#define PE_MARGINH	2

extern QColor atlantik_dpurple, atlantik_lblue, atlantik_purple, atlantik_orange,
atlantik_red, atlantik_yellow, atlantik_green, atlantik_blue, atlantik_greenbg,
atlantik_lgray, atlantik_dgray;

PortfolioView::PortfolioView(QWidget *parent, const char *name) : QWidget(parent, name)
{
	b_recreate = true;
	qpixmap = 0;

	setFixedSize(QSize(225, 75));
	setBackgroundColor(Qt::white);
	
	lname = new QLabel(this);
	lname->setAlignment(Qt::AlignLeft);
	lname->setGeometry(5, 0, width()/2, height());
	lname->setBackgroundColor(atlantik_lgray);
	lname->setMinimumSize(lname->sizeHint());
	lname->setMaximumWidth(width()-10);
	lname->setMaximumHeight(15);
	lname->show();

	lcash = new QLabel(this);
	lcash->setAlignment(Qt::AlignRight);
	lcash->setGeometry(width()/2, 0, width()-5, height());
	lcash->setBackgroundColor(atlantik_lgray);
	lcash->setMinimumSize(lcash->sizeHint());
	lcash->setMaximumWidth(width()/2);
	lcash->setMaximumHeight(15);
	lcash->show();

	QColor color;

	int i=0,j=0,x=0,y=0;
//	paint.drawRect(rect());
	for(i=39;i>=0;i--)
	{
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
			case 12: case 28:
				color = QColor(Qt::black);
			default:
				color = QColor();
		}

		// Don't ask, it works. ;-)
		switch(i)
		{
			case 3: case 9:
			case 14: case 19:
			case 24: case 29:
			case 34: case 39:
				x = PE_MARGINW + (2*PE_DISTW) + (((2*PE_DISTW)+PE_SPACE+PE_WIDTH)*(i/5));
				y = PE_MARGINH + (0*PE_DISTH) + lname->height();
				break;

			case 1: case 8:
			case 13: case 18:
			case 23: case 27:
			case 32: case 37:
				x = PE_MARGINW + (1*PE_DISTW) + (((2*PE_DISTW)+PE_SPACE+PE_WIDTH)*(i/5));
				y = PE_MARGINH + (1*PE_DISTH) + lname->height();
				break;

			case 6: case 11:
			case 16: case 21:
			case 26: case 31:
				x = PE_MARGINW + (0*PE_DISTW) + (((2*PE_DISTW)+PE_SPACE+PE_WIDTH)*(i/5));
				y = PE_MARGINH + (2*PE_DISTH) + lname->height();
				break;
			
			case 5: case 15:
			case 25: case 35:
				x = 5+((2+PE_WIDTH)*(i/10));
				y = PE_HEIGHT + PE_MARGINH + (3*PE_DISTH) + lname->height();
				+lname->height();
				break;

			case 12:
				x = ((2+PE_WIDTH)*5);
				y = PE_HEIGHT + PE_MARGINH + (3*PE_DISTH) + lname->height();
				break;

			case 28:
				x = ((2+PE_WIDTH)*6);
				y = PE_HEIGHT + PE_MARGINH + (3*PE_DISTH) + lname->height();
				break;

			default:
				x = 0; y = 0;
		}

		if (x>0 || y>0)
		{
			estate[i] = new PortfolioEstate(this);
			estate[i]->setGeometry(x, y, estate[i]->width(), estate[i]->height());
			estate[i]->setColor(color);
			estate[i]->show();
		}
		else
			estate[i]=0;
	}

	setHasTurn(false);
}

void PortfolioView::setName(const char *n)
{
	lname->setText(n);
}

void PortfolioView::setCash(const char *n)
{
	lcash->setText(n);
}

void PortfolioView::setOwned(int id, bool owned)
{
	if (estate[id]!=0)
		estate[id]->setOwned(owned);
}

void PortfolioView::setHasTurn(bool turn)
{
	if (turn==myHasTurn)
		return;

	myHasTurn = turn;

	lname->setBackgroundColor(myHasTurn ? atlantik_dgray : atlantik_lgray);
	lname->update();
	lcash->setBackgroundColor(myHasTurn ? atlantik_dgray : atlantik_lgray);
	lcash->update();

//	b_recreate = true;
//	update();
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
