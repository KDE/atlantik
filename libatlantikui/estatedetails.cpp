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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>

#include <kdialog.h>
#include <kglobalsettings.h>
#include <kicon.h>
#include <klocalizedstring.h>
#include <kpushbutton.h>
#include <KStandardGuiItem>

#include <kdebug.h>

#include <estate.h>
#include <estategroup.h>
#include <player.h>

#include "estatedetails.h"

static const int StaticTitleHeight = 50;

EstateDetails::EstateDetails(Estate *estate, const QString &text, QWidget *parent) : QWidget(parent)
{
	m_pixmap = 0;
	m_quartzBlocks = 0;
	b_recreate = true;
	m_recreateQuartz = true;

	m_estate = 0;

	m_closeButton = 0;
	m_buttons.reserve(3); // Usually there are no more than 3 action buttons

	m_mainLayout = new QVBoxLayout(this );
	Q_CHECK_PTR(m_mainLayout);

	m_mainLayout->addItem(new QSpacerItem(KDialog::spacingHint(), KDialog::spacingHint()+StaticTitleHeight, QSizePolicy::Fixed, QSizePolicy::Minimum));

	m_infoListView = new QListWidget(this);
	m_infoListView->setWordWrap(true);
	m_mainLayout->addWidget(m_infoListView);

	appendText(text);

	m_buttonBox = new QHBoxLayout();
        m_buttonBox->setSpacing(KDialog::spacingHint());
	m_mainLayout->addItem(m_buttonBox);

	m_buttonBox->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	setEstate(estate);

	connect(&m_buttonCommandMapper, SIGNAL(mapped(QString)), this, SIGNAL(buttonCommand(QString)));
}

EstateDetails::~EstateDetails()
{
	delete m_pixmap;
	delete m_quartzBlocks;
	delete m_infoListView;
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
		m_pixmap = new QPixmap(width(), height());

		QColor greenHouse(0, 255, 0);
		QColor redHotel(255, 51, 51);
		QPainter painter;
                painter.begin(m_pixmap);
                painter.initFrom(this);

		painter.setPen(Qt::black);

		painter.setBrush(m_estate ? m_estate->bgColor() : Qt::white);
		painter.drawRect(rect());

/*
		// Paint icon only when it exists and fits
		if (icon!=0 && width() > icon->width() && height() > icon->height())
			painter.drawPixmap( (width() - icon->width())/2, (height() - icon->height())/2, *icon);
*/

		if (m_estate)
		{
			QColor titleColor = (m_estate->color().isValid() ? m_estate->color() : m_estate->bgColor().light(80));

			QPixmap* quartzBuffer = new QPixmap(25, (height()/4)-2);

			QPainter quartzPainter;
			quartzPainter.begin( quartzBuffer );
                        quartzPainter.initFrom( this);

			painter.setBrush(titleColor);
			painter.drawRect(0, 0, width(), StaticTitleHeight);

			if (m_quartzBlocks)
			{
				quartzPainter.drawPixmap(0, 0, *m_quartzBlocks);
				painter.drawPixmap(1, 1, *quartzBuffer);
			}

			if (m_estate->houses() > 0)
			{
				int titleWidth = width() / 5;

				if (m_estate->houses() == 5)
				{
					// Hotel
					painter.setBrush(redHotel);
					painter.drawRect(2, 2, titleWidth-4, StaticTitleHeight-4);
				}
				else
				{
					// Houses
					painter.setBrush(greenHouse);
						int h = StaticTitleHeight-4, w = titleWidth-4;
						for ( unsigned int i=0 ; i < m_estate->houses() ; i++ )
						painter.drawRect(2+(i*(w+2)), 2, w, h);
				}
			}

			quartzPainter.end();
			delete quartzBuffer;

			// TODO: steal blur code from kicker/taskbar/taskcontainer.cpp

			// Estate name
			painter.setPen(Qt::white);
			int fontSize = KGlobalSettings::generalFont().pointSize();
			if (fontSize == -1)
				fontSize = KGlobalSettings::generalFont().pixelSize();

			painter.setFont(QFont(KGlobalSettings::generalFont().family(), fontSize * 2, QFont::Bold));
			painter.drawText(KDialog::marginHint(), KDialog::marginHint(), width()-KDialog::marginHint(), StaticTitleHeight, Qt::AlignJustify, m_estate->name());

			painter.setPen(Qt::black);

			int xText = 0;

			// Estate group
			if (m_estate->estateGroup())
			{
				xText = StaticTitleHeight - fontSize - KDialog::marginHint();
				painter.setFont(QFont(KGlobalSettings::generalFont().family(), fontSize, QFont::Bold));
				painter.drawText(5, xText, width()-10, StaticTitleHeight, Qt::AlignRight, m_estate->estateGroup()->name().toUpper());
			}

			xText = StaticTitleHeight + fontSize + 5;
			painter.setFont(QFont(KGlobalSettings::generalFont().family(), fontSize, QFont::Normal));
		}
		b_recreate = false;

	}
	QPainter painter(this);
	painter.drawPixmap(0, 0, *m_pixmap);
}

void EstateDetails::resizeEvent(QResizeEvent *)
{
	m_recreateQuartz = true;
	b_recreate = true;
}

void EstateDetails::addDetails()
{
	if (m_estate)
	{
		QListWidgetItem *infoText = 0;

		// Price
		if (m_estate->price())
		{
			infoText = new QListWidgetItem();
			infoText->setText(i18n("Price: %1", m_estate->price()));
			infoText->setIcon(KIcon("document-properties"));
			m_infoListView->addItem(infoText);
		}

		// Owner, houses, isMortgaged
		if (m_estate && m_estate->canBeOwned())
		{
			infoText = new QListWidgetItem();
			infoText->setText(i18n("Owner: %1", m_estate->owner() ? m_estate->owner()->name() : i18n("unowned")));
			infoText->setIcon(KIcon("document-properties"));
			m_infoListView->addItem(infoText);

			if (m_estate->isOwned())
			{
				infoText = new QListWidgetItem();
				infoText->setText(i18n("Houses: %1", m_estate->houses()));
				infoText->setIcon(KIcon("document-properties"));
				m_infoListView->addItem(infoText);

				infoText = new QListWidgetItem();
				infoText->setText(i18n("Mortgaged: %1", m_estate->isMortgaged() ? i18n("Yes") : i18n("No")));
				infoText->setIcon(KIcon("document-properties"));
				m_infoListView->addItem(infoText);
			}
		}
	}
}

void EstateDetails::addButton(const QString &command, const QString &caption, bool enabled)
{
	KPushButton *button = new KPushButton(caption, this);
	m_buttons.append(button);
	m_buttonCommandMapper.setMapping((QObject *)button, command);
	m_buttonBox->addWidget(button);

	if (m_estate)
	{
		QColor bgColor, fgColor;
		bgColor = m_estate->bgColor().light(110);
		fgColor = ( bgColor.red() + bgColor.green() + bgColor.blue() < 255 ) ? Qt::white : Qt::black;

		QPalette pal = button->palette();
		pal.setColor( button->foregroundRole(), fgColor );
		pal.setColor( button->backgroundRole(), bgColor );
		button->setPalette( pal );
	}
	button->setEnabled(enabled);
	button->show();

	connect(button, SIGNAL(pressed()), &m_buttonCommandMapper, SLOT(map()));
}

void EstateDetails::addCloseButton()
{
	if (!m_closeButton)
	{
		m_closeButton = new KPushButton(KStandardGuiItem::close(), this);
		m_buttonBox->addWidget(m_closeButton);
		m_closeButton->show();
		connect(m_closeButton, SIGNAL(pressed()), this, SIGNAL(buttonClose()));
	}
}

void EstateDetails::setEstate(Estate *estate)
{
	if (m_estate != estate)
	{
		m_estate = estate;

		b_recreate = true;
		update();
	}
}

void EstateDetails::setText(const QString &text)
{
	m_infoListView->clear();
	appendText(text);
}

void EstateDetails::appendText(const QString &text)
{
	if ( text.isEmpty() )
		return;

	QListWidgetItem *infoText = new QListWidgetItem();
	infoText->setText(text);
	if (text.contains(" rolls "))
		infoText->setIcon(KIcon("roll"));
	else
		infoText->setIcon(KIcon("atlantik"));
	m_infoListView->addItem(infoText);

	m_infoListView->scrollToItem(infoText);
}

void EstateDetails::clearButtons()
{
	if (m_closeButton)
	{
		delete m_closeButton;
		m_closeButton = 0;
	}

	// Delete buttons
	foreach (KPushButton *button, m_buttons)
	{
		m_buttonCommandMapper.removeMappings((QObject *)button);
		delete button;
	}
	m_buttons.resize(0); // Keep the capacity allocated
}
