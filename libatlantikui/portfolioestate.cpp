// Copyright (c) 2002 Rob Kaper <cap@capsi.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

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

void PortfolioEstate::mousePressEvent(QMouseEvent *e) 
{
	if (e->button()==LeftButton)
		emit estateClicked(m_estate);
}
