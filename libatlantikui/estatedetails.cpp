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
// the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.

#include <qpainter.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qvgroupbox.h>

#include <kdialog.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kpushbutton.h>

#include <estate.h>
#include <estategroup.h>
#include <player.h>

#include "estatedetails.h"
#include "estatedetails.moc"

EstateDetails::EstateDetails(Estate *estate, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_estate = estate;

	m_pixmap = 0;
	b_recreate = true;

	m_quartzBlocks = 0;	
	m_recreateQuartz = true;

	m_mainLayout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	Q_CHECK_PTR(m_mainLayout);

	m_mainLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));

	m_buttonBox = new QHBoxLayout(this, 0, KDialog::spacingHint());
	m_mainLayout->addItem(m_buttonBox); 

	m_buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

void EstateDetails::paintEvent(QPaintEvent *)
{
	if (m_recreateQuartz)
	{
/*
		if (m_quartzBlocks)
		{
			delete m_quartzBlocks;
			m_quartzBlocks = 0;
		}

		if (m_estate->color().isValid())
		{
			m_quartzBlocks = new KPixmap();

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
		m_pixmap = new QPixmap(width(), height());

		QColor greenHouse(0, 255, 0);
		QColor redHotel(255, 51, 51);
		QPainter painter;
		painter.begin(m_pixmap, this);

		painter.setPen(Qt::black);
		
		painter.setBrush(m_estate->bgColor());
		painter.drawRect(rect());
        
/*
		// Paint icon only when it exists and fits
		if (icon!=0 && width() > icon->width() && height() > icon->height())
			painter.drawPixmap( (width() - icon->width())/2, (height() - icon->height())/2, *icon);
*/

		int titleHeight = 50;
		QColor titleColor = (m_estate->color().isValid() ? m_estate->color() : m_estate->bgColor().light(80));

		KPixmap* quartzBuffer = new KPixmap;
		quartzBuffer->resize(25, (height()/4)-2);

		QPainter quartzPainter;
		quartzPainter.begin(quartzBuffer, this);

		painter.setBrush(titleColor);
		painter.drawRect(0, 0, width(), titleHeight);

		if (m_quartzBlocks)
		{
			quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
			painter.drawPixmap(1, 1, *quartzBuffer);
		}

		if (m_estate->houses() > 0)
		{
			int titleWidth = width()/2;

			if (m_estate->houses() == 5)
			{
				// Hotel
				painter.setBrush(redHotel);
				painter.drawRect(2, 2, titleWidth-4, titleHeight-4);
			}
			else
			{
				// Houses
				painter.setBrush(greenHouse);
				int h = titleHeight-4, w = titleWidth-4;
				for ( unsigned int i=0 ; i < m_estate->houses() ; i++ )
					painter.drawRect(2+(i*(w+2)), 2, w, h);
			}
		}

		quartzPainter.end();
		delete quartzBuffer;

		// TODO: steal blur code from kicker/taskbar/taskcontainer.cpp

		// Estate name
		painter.setPen(Qt::white);
		painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize() * 2, QFont::Bold));
		painter.drawText(KDialog::marginHint(), KDialog::marginHint(), width()-KDialog::marginHint(), titleHeight, Qt::AlignJustify, m_estate->name());

		painter.setPen(Qt::black);

		int xText = 0;
		// Estate group
		if (m_estate->estateGroup())
		{
			xText = titleHeight - KGlobalSettings::generalFont().pointSize() - KDialog::marginHint();
			painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Bold));
			painter.drawText(5, xText, width()-10, titleHeight, Qt::AlignRight, m_estate->estateGroup()->name().upper());
		}

		xText = titleHeight + KGlobalSettings::generalFont().pointSize() + 5;
		painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Normal));

		// Price
		if (m_estate->price())
		{
			painter.drawText(5, xText, i18n("Price: %1").arg(m_estate->price()));
			xText += (KGlobalSettings::generalFont().pointSize() + 5);
		}

		// Owner, houses, isMortgaged
		if (m_estate->canBeOwned())
		{
			painter.drawText(5, xText, i18n("Owner: %1").arg(m_estate->owner() ? m_estate->owner()->name() : i18n("unowned")));
			xText += (KGlobalSettings::generalFont().pointSize() + 5);

			if (m_estate->isOwned())
			{
				painter.drawText(5, xText, i18n("Houses: %1").arg(m_estate->houses()));
				xText += (KGlobalSettings::generalFont().pointSize() + 5);

				painter.drawText(5, xText, i18n("Mortgaged: %1").arg(m_estate->isMortgaged() ? i18n("Yes") : i18n("No")));
				xText += (KGlobalSettings::generalFont().pointSize() + 5);
			}
		}

		b_recreate = false;
	}
	bitBlt(this, 0, 0, m_pixmap);
}

void EstateDetails::resizeEvent(QResizeEvent *)
{
	m_recreateQuartz = true;
	b_recreate = true;
}

void EstateDetails::addButton(QString command, QString caption, bool enabled)
{
	KPushButton *button = new KPushButton(caption, this);
	m_buttonCommandMap[(QObject *)button] = command;
	m_buttonBox->addWidget(button);

	button->setEnabled(enabled);
	button->show();

	connect(button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
}

void EstateDetails::buttonPressed()
{
	emit buttonCommand(QString(m_buttonCommandMap[(QObject *)QObject::sender()]));
}
