// Copyright (c) 2002-2004 Rob Kaper <cap@capsi.com>
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

#include <qpainter.h>
#include <qpixmap.h>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QFontDatabase>
#include <QPaintEvent>
#include <QResizeEvent>

#include <estate.h>
#include <estategroup.h>
#include <player.h>

#include "estatedetailsbase.h"

static const int StaticTitleHeight = 50;

EstateDetailsBase::EstateDetailsBase(Estate *estate, QWidget *parent)
	: QWidget(parent)
	, m_estate(nullptr)
	, m_pixmap(nullptr)
	, m_quartzBlocks(nullptr)
	, b_recreate(true)
	, m_recreateQuartz(true)
{
	setAutoFillBackground(true);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	mainLayout->addItem(new QSpacerItem(5, StaticTitleHeight, QSizePolicy::Fixed, QSizePolicy::Minimum));

	m_widget = new QWidget(this);
	mainLayout->addWidget(m_widget);

	setEstate(estate);

	// If 'estate' is not null, setEstate initialized the palette with
	// its color
	if (!estate)
	{
		QPalette pal = palette();
		pal.setColor(backgroundRole(), Qt::white);
		setPalette(pal);
	}
}

EstateDetailsBase::~EstateDetailsBase()
{
	delete m_pixmap;
	delete m_quartzBlocks;
}

void EstateDetailsBase::paintEvent(QPaintEvent *e)
{
	if (m_recreateQuartz)
	{
/*
		if (m_quartzBlocks)
		{
			delete m_quartzBlocks;
			m_quartzBlocks = nullptr;
		}

		if (m_estate->color().isValid())
		{
			m_quartzBlocks = new QPixmap();

			if (m_orientation == North || m_orientation == South)
				m_quartzBlocks->resize(25, m_titleHeight-2);
			else
				m_quartzBlocks->resize(25, m_titleWidth-2);

			drawQuartzBlocks(m_quartzBlocks, *m_quartzBlocks, m_estate->color().light(60), m_estate->color());
			m_quartzBlocks = rotatePixmap(m_quartzBlocks);
		}
*/
		m_recreateQuartz = false;
		b_recreate = true;
	}

	if (b_recreate)
	{
		delete m_pixmap;
		m_pixmap = nullptr;

		if (m_estate)
		{
			m_pixmap = new QPixmap(width(), StaticTitleHeight);

			QColor greenHouse(0, 255, 0);
			QColor redHotel(255, 51, 51);
			QPainter painter;
			painter.begin(m_pixmap);

			painter.setPen(Qt::black);

			QColor titleColor = (m_estate->color().isValid() ? m_estate->color() : m_estate->bgColor().lighter(80));
			const int leftMarginHint = QApplication::style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
			const int topMarginHint = QApplication::style()->pixelMetric(QStyle::PM_LayoutTopMargin);
			const int spacingHint = QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
			const QFont generalFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);

			painter.setBrush(titleColor);
			painter.drawRect(0, 0, width()-1, StaticTitleHeight-1);

			if (m_quartzBlocks)
			{
				painter.drawPixmap(1, 1, *m_quartzBlocks);
			}

			if (m_estate->houses() > 0)
			{
				int titleWidth = width() / 5;

				if (m_estate->houses() == 5)
				{
					// Hotel
					painter.setBrush(redHotel);
					painter.drawRect(2, 2, titleWidth-4-1, StaticTitleHeight-4-1);
				}
				else
				{
					// Houses
					painter.setBrush(greenHouse);
						int h = StaticTitleHeight-4, w = titleWidth-4;
						for ( unsigned int i=0 ; i < m_estate->houses() ; i++ )
						painter.drawRect(2+(i*(w+2)), 2, w-1, h-1);
				}
			}

			// TODO: steal blur code from kicker/taskbar/taskcontainer.cpp

			// Estate name
			painter.setPen(Qt::white);
			int fontSize = generalFont.pointSize();
			if (fontSize == -1)
				fontSize = generalFont.pixelSize();

			painter.setFont(QFont(generalFont.family(), fontSize * 2, QFont::Bold));
			painter.drawText(leftMarginHint, topMarginHint, width()-leftMarginHint, StaticTitleHeight, Qt::AlignJustify, m_estate->name());

			painter.setPen(Qt::black);

			int xText = 0;

			// Estate group
			if (m_estate->estateGroup())
			{
				xText = StaticTitleHeight - fontSize - spacingHint;
				painter.setFont(QFont(generalFont.family(), fontSize, QFont::Bold));
				painter.drawText(5, xText, width()-10, StaticTitleHeight, Qt::AlignRight, m_estate->estateGroup()->name().toUpper());
			}

			xText = StaticTitleHeight + fontSize + 5;
			painter.setFont(QFont(generalFont.family(), fontSize, QFont::Normal));
		}
		b_recreate = false;

	}
	QPainter painter(this);
	// Draw the border, as simple polyline
	painter.setPen(Qt::black);
	const QPointF points[5] = {
		QPointF(0, 0),
		QPointF(0, height() - 1),
		QPointF(width() - 1, height() - 1),
		QPointF(width() - 1, 0),
		QPointF(0, 0),
	};
	painter.drawPolyline(points, sizeof(points)/sizeof(points[0]));
	// Draw the title, if its area was damaged
	if (m_pixmap && e->rect().intersects(QRect(0, 0, width(), StaticTitleHeight)))
		painter.drawPixmap(0, 0, *m_pixmap);
}

void EstateDetailsBase::resizeEvent(QResizeEvent *e)
{
	if (e->oldSize().width() != e->size().width())
	{
		m_recreateQuartz = true;
		b_recreate = true;
	}
}

void EstateDetailsBase::setEstate(Estate *estate)
{
	if (m_estate != estate)
	{
		m_estate = estate;

		QPalette pal = palette();
		pal.setColor(backgroundRole(), m_estate ? m_estate->bgColor() : Qt::white);
		setPalette(pal);

		b_recreate = true;
		update();
	}
}

#include "moc_estatedetailsbase.cpp"
