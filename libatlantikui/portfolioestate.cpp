#include <qcolor.h>
#include <qpainter.h>
#include <qrect.h>

#include "portfolioestate.moc"
#include "estate.h"

PortfolioEstate::PortfolioEstate(Estate *estate, Player *player, bool alwaysOwned, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_estate = estate;
	m_player = player;
	m_alwaysOwned = alwaysOwned;

#warning add old Qcolor properties as arguments

    QSize s(PE_WIDTH, PE_HEIGHT);
    setFixedSize(s);

    b_recreate = true;
}

void PortfolioEstate::estateChanged()
{
	b_recreate = true;
	update();
}

QPixmap PortfolioEstate::drawPixmap(Estate *estate, Player *player, bool alwaysOwned)
{
	QColor lightGray(204, 204, 204), darkGray(153, 153, 153);
	QPixmap qpixmap(PE_WIDTH, PE_HEIGHT);
	QPainter painter;
	painter.begin(&qpixmap);

	painter.setPen(lightGray);
	painter.setBrush(white);
	painter.drawRect(QRect(0, 0, PE_WIDTH, PE_HEIGHT));
	if (alwaysOwned || (estate && estate->isOwned() && player == estate->owner()))
	{
		painter.setPen(darkGray);
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

		painter.setPen(estate->color());	
		painter.setBrush(estate->color());
	}
	else
	{
		painter.setPen(lightGray);	
		painter.setBrush(lightGray);
	}
	painter.drawRect(0, 0, PE_WIDTH, 3);

	return qpixmap;
}

void PortfolioEstate::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		m_pixmap = drawPixmap(m_estate, m_player, m_alwaysOwned);
		b_recreate = false;
	}
	bitBlt(this, 0, 0, &m_pixmap);
}
