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

#include <QColor>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPaintEvent>

#include "portfolioestate.h"
#include "estate.h"

PortfolioEstate::PortfolioEstate(Estate *estate, Player *player, bool alwaysOwned, QWidget *parent)
	: QWidget(parent)
	, m_estate(estate)
	, m_player(player)
	, b_recreate(true)
	, m_alwaysOwned(alwaysOwned)
{
    QSize s(PE_WIDTH, PE_HEIGHT);
    setFixedSize(s);

	// react to changes in the estate only when we do not draw it
	// as always owned
	if (!m_alwaysOwned)
	{
		m_estateColor = m_estate->color();
		m_estateOwner = m_estate->owner();
		connect(m_estate, SIGNAL(changed()), this, SLOT(estateChanged()));
	}
}

void PortfolioEstate::estateChanged()
{
	// if we get here, then we are not assuming the estate is
	// always owned

	// update if the color changed
	if (m_estate->color() != m_estateColor)
	{
		m_estateColor = m_estate->color();
		b_recreate = true;
	}

	// update if the owner changed ...
	if (m_estate->owner() != m_estateOwner)
	{
		Player *prevOwner = m_estateOwner;
		m_estateOwner = m_estate->owner();

		// ... but only if the previous owner or the new one
		// is the own player
		if (m_estate->owner() == m_player || prevOwner == m_player)
			b_recreate = true;
	}

	if (b_recreate)
		update();
}

QPixmap PortfolioEstate::drawPixmap(Estate *estate, Player *player, bool alwaysOwned)
{
	QColor lightGray(204, 204, 204), darkGray(153, 153, 153);
	QPixmap qpixmap(PE_WIDTH, PE_HEIGHT);
	QPainter painter;
	painter.begin(&qpixmap);

	painter.setPen(lightGray);
	painter.setBrush(Qt::white);
	painter.drawRect(QRect(0, 0, PE_WIDTH-1 , PE_HEIGHT-1));
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
	painter.drawRect(0, 0, PE_WIDTH-1, 3-1);

	return qpixmap;
}

void PortfolioEstate::paintEvent(QPaintEvent *e)
{
	if (b_recreate)
	{
		m_pixmap = drawPixmap(m_estate, m_player, m_alwaysOwned);
		b_recreate = false;
	}
	QPainter painter(this);
	painter.drawPixmap(e->rect(), m_pixmap, e->rect());
}

void PortfolioEstate::mousePressEvent(QMouseEvent *e)
{
	if (e->button()==Qt::LeftButton)
		Q_EMIT estateClicked(m_estate);
}

#include "moc_portfolioestate.cpp"
