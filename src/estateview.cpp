#include <qtooltip.h>
#include <qpainter.h>
#include <qtimer.h>

#warning remove
#include <iostream.h>

#include "estateview.h"

extern QColor kmonop_greenbg, kmonop_greenhouse;

EstateView::EstateView(int _orientation, const QColor &_color, QWidget *parent, const char *name) : QWidget(parent, name)
{
	orientation = _orientation;
	color = _color;

	setBackgroundMode(NoBackground); // avoid flickering

	b_recreate = true;
	qpixmap = 0;
	pe = 0;

/*
	lname = new QLabel(this);
	lname->setAlignment(Qt::AlignLeft);
	lname->setMinimumSize(lname->sizeHint());
	lname->setMaximumWidth(width());
	lname->setMaximumHeight(15);
	lname->hide();
*/
	setName("Boardwalk");
	setHouses(0);

	QToolTip::add(this, estatename);
}

void EstateView::setName(const char *n)
{
	estatename.setLatin1(n, strlen(n));
	QToolTip::remove(this);
	QToolTip::add(this, estatename);
/*
	lname->setText(n);
*/
}

void EstateView::setHouses(int _houses)
{
	houses = _houses;
}

void EstateView::setOwned(bool owned)
{
	if (owned)
	{
		delete pe;
		pe = 0;
	}
	else
	{
		if (pe==0)
		{
			// Display a coloured portfolioestate in the center to indicate
			// property is for sale
			pe = new PortfolioEstate(this);
			pe->setColor(color);
			pe->setOwned(true);
			cout << (width()/2) << " - " << (pe->width()/2) << " - " << (height()/2) << " - " << (pe->height()/2) << endl;
			pe->setGeometry((width()/2) - (pe->width()/2), (height()/2) - (pe->height()/2), pe->width(), pe->height());
			pe->show();
		}
		else if (!pe->isVisible())
			pe->show();
	}
}

void EstateView::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(Qt::black);
		painter.setBrush(kmonop_greenbg);
		painter.drawRect(rect());

		if (color.isValid())
		{
			painter.setBrush(color);
			switch(orientation)
			{
				case North:
					painter.drawRect(0, 0, width(), height()/4);
//					painter.setPen(Qt::black);
//					painter.setFont(QFont("helvetica", 10));
//					painter.drawText(0, height()/4, width(), height()/2, (Qt::AlignHCenter | Qt::AlignTop), estatename, estatename.length());
					if (houses>0)
					{
						if (houses == 5)
						{
						}
						else
						{
							painter.setBrush(kmonop_greenhouse);
							int h = (height()/4)-4, w = (width()/4)-2;
							for( int i=0 ; i<houses ; i++ )
							{
								painter.drawRect(2+(i*(w+2)), 2, w, h);
							}
						}
					}
					break;
				case South:
					painter.drawRect(0, height()-(height()/4), width(), height()/4);
					break;
				case West:
					painter.drawRect(0, 0, width()/4, height());
					break;
				case East:
					painter.drawRect(width()-(width()/4), 0, width()/4, height());
					break;
			}
		}
		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void EstateView::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
	
	QTimer::singleShot(0, this, SLOT(slotResizeAftermath()));
}

void EstateView::slotResizeAftermath()
{
	if (pe!=0)
		pe->setGeometry((width()/2) - (pe->width()/2), (height()/2) - (pe->height()/2), pe->width(), pe->height());
}
