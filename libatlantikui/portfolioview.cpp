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
#include <qcursor.h>

#include <kdialogbase.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpopupmenu.h>

#include <atlantic_core.h>
#include <config.h>
#include <player.h>
#include <estate.h>
#include <estategroup.h>

#include "portfolioview.moc"

#define PE_DISTW	4
#define	PE_DISTH	4
#define PE_SPACE	2
#define PE_MARGINW	5
#define PE_MARGINH	2

PortfolioView::PortfolioView(AtlanticCore *core, Player *player, QColor activeColor, QColor inactiveColor, QWidget *parent, const char *name) : QWidget(parent, name)
{
	m_atlanticCore = core;
	m_player = player;
	m_activeColor = activeColor;
	m_inactiveColor = inactiveColor;
	m_lastPE = 0;

	setBackgroundColor(Qt::white);

	qpixmap = 0;
	b_recreate = true;
	
	setMinimumHeight(35);
}

Player *PortfolioView::player()
{
	return m_player;
}

void PortfolioView::buildPortfolio()
{
	// TODO: clear current portfolioEstateMap
	// Loop through estate groups in order
	QPtrList<EstateGroup> estateGroups = m_atlanticCore->estateGroups();
	PortfolioEstate *lastPE = 0, *firstPEprevGroup = 0;

	int x = 100, y = 25, marginHint = 5, bottom;
	bottom = 20 - PE_HEIGHT - marginHint;

	EstateGroup *estateGroup;
	for (QPtrListIterator<EstateGroup> it(estateGroups); *it; ++it)
	{
		if ((estateGroup = *it))
		{
			// New group
			lastPE = 0;

			// Loop through estates
			QPtrList<Estate> estates = m_atlanticCore->estates();
			Estate *estate;
			for (QPtrListIterator<Estate> it(estates); *it; ++it)
			{
				if ((estate = *it) && estate->estateGroup() == estateGroup)
				{
					// Create PE
					PortfolioEstate *portfolioEstate = new PortfolioEstate(estate, m_player, false, this, "portfolioestate");
					portfolioEstateMap[estate->estateId()] = portfolioEstate;

 					connect(portfolioEstate, SIGNAL(estateClicked(Estate *)), this, SIGNAL(estateClicked(Estate *)));
					if (lastPE)
					{
						x = lastPE->x() + 2;
						y = lastPE->y() + 4;
						if (y > bottom)
							bottom = y;
					}
					else if (firstPEprevGroup)
					{
						x = firstPEprevGroup->x() + PE_WIDTH + 8;
						y = 20 + marginHint;
						firstPEprevGroup = portfolioEstate;
					}
					else
					{
						x = marginHint;
						y = 20 + marginHint;
						if (y > bottom)
							bottom = y;
						firstPEprevGroup = portfolioEstate;
					}

					portfolioEstate->setGeometry(x, y, portfolioEstate->width(), portfolioEstate->height());
					portfolioEstate->show();

					connect(estate, SIGNAL(changed()), portfolioEstate, SLOT(estateChanged()));

					lastPE = portfolioEstate;
				}
			}
		}
	}
	setMinimumWidth(x + PE_WIDTH + marginHint);
	int minHeight = bottom + PE_HEIGHT + marginHint;
	if (minHeight > minimumHeight())
		setMinimumHeight(minHeight);
}

void PortfolioView::paintEvent(QPaintEvent *)
{
	if (b_recreate)
	{
		delete qpixmap;
		qpixmap = new QPixmap(width(), height());

		QPainter painter;
		painter.begin(qpixmap, this);

		painter.setPen(Qt::white);
		painter.setBrush(Qt::white);
		painter.drawRect(rect());

		painter.setPen(m_player->hasTurn() ? m_activeColor : Qt::black);
		painter.setBrush(m_player->hasTurn() ? m_activeColor : Qt::black);
		painter.drawRect(0, 0, width(), 20);

		painter.setPen(Qt::white);
		painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Bold));
		painter.drawText(KDialog::marginHint(), 15, m_player->name());
		painter.drawText(width() - 50, 15, QString::number(m_player->money()));

		if (!portfolioEstateMap.size())
		{
			painter.setPen(Qt::black);
			painter.setBrush(Qt::white);

			painter.setFont(QFont(KGlobalSettings::generalFont().family(), KGlobalSettings::generalFont().pointSize(), QFont::Normal));
			painter.drawText(KDialog::marginHint(), 30, m_player->host());
		}

		b_recreate = false;
	}
	bitBlt(this, 0, 0, qpixmap);
}

void PortfolioView::resizeEvent(QResizeEvent *)
{
	b_recreate = true;
}

void PortfolioView::playerChanged()
{
	b_recreate = true;
	update();
}

void PortfolioView::mousePressEvent(QMouseEvent *e) 
{
	if (e->button()==RightButton && !m_player->isSelf())
	{
		KPopupMenu *rmbMenu = new KPopupMenu(this);
		rmbMenu->insertTitle(m_player->name());

		// Start trade
		rmbMenu->insertItem(i18n("Request Trade with %1").arg(m_player->name()), 0);

		connect(rmbMenu, SIGNAL(activated(int)), this, SLOT(slotMenuAction(int)));
		QPoint g = QCursor::pos();
		rmbMenu->exec(g);
	}
}

void PortfolioView::slotMenuAction(int item)
{
	switch (item)
	{
	case 0:
		emit newTrade(m_player);
		break;
	}
}
