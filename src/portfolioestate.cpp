#include <qpainter.h>
#include <qcolor.h>

#include "portfolioestate.moc"
#include "estate.h"

extern QColor atlantik_lgray, atlantik_dgray;

PortfolioEstate::PortfolioEstate(Estate *estate, bool alwaysOwned, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_estate = estate;
	m_alwaysOwned = alwaysOwned;

    QSize s(PE_WIDTH, PE_HEIGHT);
    setFixedSize(s);
    b_recreate = true;
    qpixmap=0;
}

void PortfolioEstate::estateChanged()
{
	b_recreate = true;
	update();
}

void PortfolioEstate::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());
		
		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(atlantik_lgray);
		painter.setBrush(white);
		painter.drawRect(rect());
		if (m_alwaysOwned || m_estate->isOwned())
		{
			painter.setPen(atlantik_dgray);
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

			painter.setPen(m_estate->color());	
			painter.setBrush(m_estate->color());
		}
		else
		{
			painter.setPen(atlantik_lgray);	
			painter.setBrush(atlantik_lgray);
		}
		painter.drawRect(0,0,width(),3);
		
		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}
